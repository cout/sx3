#!/bin/sh

if [ "$1" == "" ]; then

  echo "Usage: cvslogin <username>"

else

  CVS_RSH=ssh
  #CVSROOT=:ext:$1@sx3.dhs.org:/cvs/sx3
  CVSROOT=:ext:$1@sx3.dhs.org:/cvs

  export CVS_RSH CVSROOT

  echo "Finished.  Please check your environment variables to make sure that"
  echo "CVS_RSH and CVSROOT are set properly."

fi;
