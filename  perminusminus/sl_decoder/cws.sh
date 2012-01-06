#./sl_decoder ctb5/cws.bin models/cws_dat.bin models/cws_label_info.txt $1 $2 $3 $4 $5
./sl_decoder -D ctb5/seg.model ctb5/seg.dat ctb5/seg.label_index $*

