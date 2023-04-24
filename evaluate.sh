#!/bin/bash
#!/bin/bash

if [ $1 -eq 0 ]; then
     echo -------------------------------------------------- EVALUATE.SH: SOFTWARE ACTS APPLICATION RUNNING --------------------------------------------------
	 XWARE="SW"
	 RUNNING_SYNTHESIS=0
elif [ $1 -eq 1 ]; then
     echo -------------------------------------------------- EVALUATE.SH: HARDWARE ACTS APPLICATION RUNNING --------------------------------------------------
	 XWARE="HW" 
	 RUNNING_SYNTHESIS=0
elif [ $1 -eq 2 ]; then
     echo -------------------------------------------------- EVALUATE.SH: HARDWARE ACTS SYNTHESIS RUNNING --------------------------------------------------
	 XWARE="HW" 
	 RUNNING_SYNTHESIS=1
else
     echo -------------------------------------------------- EVALUATE.SH: CMD ARGS NOT DEFINED. EXITING.... --------------------------------------------------
	 quit
fi

make generatesrcs XWARE=$XWARE RUNNING_SYNTHESIS=$RUNNING_SYNTHESIS NUM_PEs=12 NUM_FPGAs=$2								

echo 'finished: successfully finished all processing'





