#!/bin/sh

echo "Please be sure to run this program as \"source cvslogin <username>\","
echo "otherwise your environment variables may not be set correctly."

if [ "$1" == "" ]; then

  echo "Usage: cvslogin <username>"

else

  CVS_RSH=ssh
  CVSROOT=:ext:$1@sx3.dhs.org:/cvs/sx3

  export CVS_RSH CVSROOT

  echo "Finished.  Please check your environment variables to make sure that"
  echo "CVS_RSH and CVSROOT are set properly."

fi;
