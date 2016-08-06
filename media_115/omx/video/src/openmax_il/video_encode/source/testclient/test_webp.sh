#!/bin/bash

# SET THIS
# Choose whether to run test in ARM simulator (1) or linux executable (0)
simulator=0
#set -x

# SET THIS
# Choose whether to run all test like defined in parameter.sh (0) or all cases run in "whole frame" mode (1).
encodeType=0

#WEBPDEC=webpdec

if [ -s "test_data_parameter_webp.sh" ] ; then
    TEST_PARAMETER=test_data_parameter_webp.sh
else
    TEST_PARAMETER=customer_data_parameter_webp.sh
fi

# Encoder bin
if [ $simulator == 1 ]; then
    enc_file=testenc.axf
    enc_bin="armsd -cpu ARM926EJ-S -clock 266M -script armscript.txt $enc_file"
    if [ ! -e $enc_file ]; then
        echo "ARM image $enc_file not found. Compile with 'make pclinuxarm'"
        exit
    fi
else
    enc_bin=./omxenctest
    if [ ! -e $enc_bin ]; then
        echo "Executable $enc_bin not found. Compile with 'make'"
        exit
    fi

    ref_enc_bin=$SYSTEM_MODEL_HOME/vp8_testenc
    if [ ! -e $ref_enc_bin ]; then
        echo "Executable $ref_enc_bin not found. Compile with 'make'"
        exit
    fi
fi

# Output file when running all cases
# .log and .txt are created
resultfile=results_webp

# Current date
date=`date +"%d.%m.%y %k:%M"`

# Check parameter
if [ $# -eq 0 ]; then
    echo "Usage: test_webp.sh <test_case_number>"
    echo "   or: test_webp.sh <all>"
    echo "   or: test_webp.sh <first_case> <last_case>"
    exit
fi

# Encoder command
# For armsd-simulator the command line must be less than 300 characters
encode() {
    omxRotation=0
    case "${rotation[$set_nro]}" in
        1) {
            omxRotation=90
            };;
        2) {
            omxRotation=270
            };;
        * )
    esac

    if [ ${codingtype[${set_nro}]} -eq 1 ] ; then
        #sliced mode
        restartInterval=${nmcu[$set_nro]}
        case "${frametype[$set_nro]}" in
            0|1) {
                #yuv420 planar | yuv420 semi planar
                omxSlicing=-S' '$[${nmcu[$set_nro]}*16]
                let bufferSize=${width_orig[${set_nro}]}*${nmcu[$set_nro]}*16*3/2
                  };;
            2|3|4|5|6|8) {
                #ycbycr | cbycry | rgb565 | bgr565 | rgb555 | rgb444
                omxSlicing=-S' '$[${nmcu[$set_nro]}*16]
                let bufferSize=${width_orig[${set_nro}]}*${nmcu[$set_nro]}*16*2
                  };;
            10) {
                #rgb888 | bgr888 
                omxSlicing=-S' '$[${nmcu[$set_nro]}*16]
                let bufferSize=${width_orig[${set_nro}]}*${nmcu[$set_nro]}*16*4   
                  };;
            * )
        esac
    else
        #whole frame mode
        restartInterval=0
        omxSlicing=''
        case "${frametype[$set_nro]}" in
            0|1) {
                #yuv420 planar | yuv420 semi planar
                let bufferSize=${width_orig[${set_nro}]}*${height_orig[${set_nro}]}*3/2
                  };;
            2|3|4|5|6|8) {
                #ycbycr | cbycry | rgb565 | bgr565 | rgb555 | rgb444 | yuv422 planar
                let bufferSize=${width_orig[${set_nro}]}*${height_orig[${set_nro}]}*2
                  };;
            10) {
                #rgb888 | bgr888 
                let bufferSize=${width_orig[${set_nro}]}*${height_orig[${set_nro}]}*4   
                  };;
            * )
        esac
    fi
    
    # limitation of component
    if [ ${bufferSize} -lt 38016 ] ; then
        bufferSize=38016
    fi

    qValues=(120 108 96 84 72 60 48 24 12 0)

    #reference data
    $ref_enc_bin \
    --input ${input[${set_nro}]} \
    --output ${case_dir}/${output[${set_nro}]} \
    --qpHdr ${qValues[${qtable[${set_nro}]}]} \
    --firstPic 0 \
    --lastPic$ ${lastPic[${set_nro}]} \
    --lumWidthSrc ${width_orig[${set_nro}]} \
    --lumHeightSrc ${height_orig[${set_nro}]} \
    --width ${width_crop[${set_nro}]} \
    --height ${height_crop[${set_nro}]} \
    --horOffsetSrc ${horoffset[${set_nro}]} \
    --verOffsetSrc ${veroffset[${set_nro}]} \
    --inputFormat ${frametype[${set_nro}]} \
    --rotation ${rotation[${set_nro}]}

    ${enc_bin} \
    -O webp \
    -s ${bufferSize} \
    -i ${input[${set_nro}]} \
    -o ${output[${set_nro}]} \
    -q ${qtable[${set_nro}]} \
    -a 0 \
    -b $[${lastPic[${set_nro}]}+1] \
    -w ${width_orig[${set_nro}]} \
    -h ${height_orig[${set_nro}]} \
    -x ${width_crop[${set_nro}]} \
    -y ${height_crop[${set_nro}]} \
    -X ${horoffset[${set_nro}]} \
    -Y ${veroffset[${set_nro}]} \
    -l ${frametype[${set_nro}]} \
    -r ${omxRotation} \
       ${omxSlicing}
}

encode_whole_mode() {
    omxRotation=0
    case "${rotation[$set_nro]}" in
        1) {
            omxRotation=90
            };;
        2) {
            omxRotation=270
            };;
        * )
    esac

    bufferSize=1100000
    if [ ${codingtype[${set_nro}]} -eq 1 ] ; then
        bufferSize=${buffer_size[${set_nro}]}

        # limitation of component
        if [ ${bufferSize} -lt 38016 ] ; then
            bufferSize=38016
        fi

        case "${frametype[$set_nro]}" in
            0|1) {
                let width=${width_orig[${set_nro}]}+${width_orig[${set_nro}]}/2
                let omxSlicing=${buffer_size[${set_nro}]}/$width
                #if YUV422 planar
                if [ ${frametype[${set_nro}]}  -eq 10 ]; then
                    # round to nearest (smaller than) multiplier of 8
                    let omxSlicingTmp=${omxSlicing}/8*8
                else
                    # round to nearest (smaller than) multiplier of 8
                    let omxSlicingTmp=${omxSlicing}/16*16
                fi
                omxSlicing=-S' '${omxSlicingTmp}
                };;
            2|3) {
                let width=${width_orig[${set_nro}]}*2
                let omxSlicing=${buffer_size[${set_nro}]}/$width
                #if YUV422 planar
                if [ ${frametype[${set_nro}]}  -eq 10 ]; then
                    # round to nearest (smaller than) multiplier of 8
                    let omxSlicingTmp=${omxSlicing}/8*8
                else
                    # round to nearest (smaller than) multiplier of 8
                    let omxSlicingTmp=${omxSlicing}/16*16
                fi
                omxSlicing=-S' '${omxSlicingTmp}
                };;
            * )
        esac
    fi

    #reference data
    $ref_enc_bin \
    --input ${input[${set_nro}]} \
    --output ${case_dir}/${output[${set_nro}]} \
    --firstPic 0 \
    --lastPic ${lastPic[${set_nro}]} \
    --lumWidthSrc ${width_orig[${set_nro}]} \
    --lumHeightSrc ${height_orig[${set_nro}]} \
    --width ${width_crop[${set_nro}]} \
    --height ${height_crop[${set_nro}]} \
    --horOffsetSrc ${horoffset[${set_nro}]} \
    --verOffsetSrc ${veroffset[${set_nro}]} \
    --inputFormat ${frametype[${set_nro}]} \
    --rotation ${rotation[${set_nro}]}

    ${enc_bin} \
    -O webp \
    -s ${bufferSize} \
    -i ${input[${set_nro}]} \
    -o ${output[${set_nro}]} \
    -q ${qtable[${set_nro}]} \
    -a 0 \
    -b $[${lastPic[${set_nro}]}+1] \
    -w ${width_orig[${set_nro}]} \
    -h ${height_orig[${set_nro}]} \
    -x ${width_crop[${set_nro}]} \
    -y ${height_crop[${set_nro}]} \
    -X ${horoffset[${set_nro}]} \
    -Y ${veroffset[${set_nro}]} \
    -l ${frametype[${set_nro}]} \
    -r ${omxRotation} \
       ${omxSlicing}
}

compare() {
    if [ -s "stream.webp" ]; then
        if cmp --ignore-initial=20:44 stream.webp $case_dir/stream.webp; then
            echo "Case $case_nro OK"
            echo "E case_$case_nro;SW/HW Integration;;$date;OK;;$hwtag;$USER;;;$swtag;$systag"
        else
            echo "Case $case_nro FAILED"
            echo "E case_$case_nro;SW/HW Integration;;$date;Failed;;$hwtag;$USER;;;$swtag;$systag"
            if [ -s "$WEBPDEC" ]; then
                $WEBPDEC  $case_dir/stream.webp >> ref.log
                mv out.yuv ref.yuv
                $WEBPDEC stream.webp >> omx.log
                mv out.yuv omx.yuv

                if cmp ref.yuv omx.yuv; then
                    echo "failed, Decoded outputs match"
                else
                    echo "failed, decoded outputs differ"
                fi
            fi

    fi
    else
        echo "Case $case_nro FAILED"
        echo "E case_$case_nro;SW/HW Integration;;$date;Failed;;$hwtag;$USER;;;$swtag;$systag"

        echo "failed, stream.webp size 0 or file not found"
    fi
}

test_set() {
    # Test data dir
    let "set_nro=${case_nro}-${case_nro}/5*5"
    case_dir=$TEST_DATA_HOME/case_$case_nro

    # Do nothing if test case is not valid
    if [ ${valid[${set_nro}]} -eq -1 ]; then
        echo "case_$case_nro is not valid."
    else
        # Do nothing if test data doesn't exists
        if [ ! -e $case_dir ]; then
            mkdir $case_dir
        fi

        echo "========================================================="
        echo "Run case $case_nro"

        rm -f mb_control.bin rlc.bin mb_controlTn.bin rlcTn.bin stream.webp

        # Run encoder
        if [ $encodeType -eq 0 ];
        then
            echo "NOTE! USES CODINGTYPE DEFINED IN PARAMETER.SH"
            encode > encoder_webp.log
            cat encoder_webp.log
        else
            echo "NOTE! ALL CASES IN WHOLE FRAME MODE"
            encode_whole_mode > encoder_webp.log
            cat encoder_webp.log
        fi

        if [ $simulator == 1 ]; then
            cp encoder_webp.log sim_case_$case_nro.log
            cp tmp.prf sim_case_$case_nro.prf
        fi

        # Compare stream to reference
        compare

        echo "case_${case_nro} DONE"
        mv stream.webp ENC_CASE_${case_nro}.webp
    fi
}

run_cases() {
    echo "Running test cases $first_case..$last_case"
    echo "This will take several minutes"
    echo "Output is stored in $resultfile.log and $resultfile.txt"
    rm -f $resultfile.log $resultfile.txt
    for ((case_nro=$first_case; case_nro<=$last_case; case_nro++))
    do
        echo -en "\rCase $case_nro "
        . $TEST_PARAMETER "$case_nro"
        test_set >> $resultfile.log
    done
    echo -e "\nDone\n"
    grep "E case_" $resultfile.log > $resultfile.csv
    grep "Case" $resultfile.log > $resultfile.txt
    cat $resultfile.txt
}

run_case() {
    . $TEST_PARAMETER "$case_nro"
    test_set
}

# Encoder test set.
trigger=-1 # NO Logic analyzer trigger by default

case "$1" in
    all)
    first_case=4000
    last_case=4004
    run_cases;;

    *)
    if [ $# -eq 1 ]; then
        case_nro=$1
        run_case
    else
        first_case=$1
        last_case=$2
        run_cases
    fi;;
esac

