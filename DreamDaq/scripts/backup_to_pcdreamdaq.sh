/home/dreamtest/globus-connect-personal/globusconnectpersonal-2.2.0/globusconnectpersonal -start &
sleep 5
ssh -t dreamdaq@cli.globusonline.org transfer -s 3 -- TTU-Sigmorgh-DREAM/lustre/hep/osg/dream/2015/ pcdreamdaqpersonal/~/dreamix2/2015/ -r
