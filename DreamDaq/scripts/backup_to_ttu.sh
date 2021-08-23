/home/dreamtest/globusconnectpersonal/globusconnectpersonal -start &
sleep 5
ssh -t dreamdaq@cli.globusonline.org transfer -s 3 -- pcdreamdaq2personal/~/storage/ TTU-Sigmorgh-DREAM/lustre/hep/osg/dream/2017/ -r
