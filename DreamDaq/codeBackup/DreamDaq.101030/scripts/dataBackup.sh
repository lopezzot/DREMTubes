
MIN=$1
MAX=$2
BAKDIR=/home/dreamtest/databackup
PRE=/home/dreamtest/working/data

LIST=`echo ${PRE}/datafile_run$MIN.dat; \
      find ${PRE} -newer ${PRE}/datafile_run$MIN.dat -not -newer ${PRE}/datafile_run$MAX.dat -size +1M`

echo $LIST

echo "Backing up these files:"
echo "$LIST"
echo 'Press <CTRL-C> to abort, <return> to continue'
read

for i in $LIST; 
  do
    echo "gzip -c $i > ${BAKDIR}/`basename $i`.gz"  
    gzip -c $i > ${BAKDIR}/`basename $i`.gz
done

