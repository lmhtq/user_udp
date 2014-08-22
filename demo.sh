#!/bin/sh
#the root folder of vlc's source code
#and the path variable must end with '/'

VLC_ROOT=~/codes/vlc-2.1.4/vlc-2.1.4/
PWD=$(cd "$(dirname "$0")"; pwd)
codec_dir=${VLC_ROOT}modules/codec
rtp_dir=${VLC_ROOT}modules/access/rtp

if [ ! -d $VLC_ROOT ]; then
	echo "Cannot detect the vlc source codes"
	exit
fi

if [ ! -f $codec_dir/x264.c.bak ]; then  
mv $codec_dir/x264.c $codec_dir/x264.c.bak
fi

if [ ! -f $rtp_dir/session.c.bak ]; then  
mv $rtp_dir/session.c $rtp_dir/session.c.bak
fi

if [ -L $codec_dir/x264.c ]; then  
rm $codec_dir/x264.c
ln -s $PWD/x264.c $codec_dir/x264.c
else
ln -s $PWD/x264.c $codec_dir/x264.c
fi

if [ -L $rtp_dir/session.c ]; then  
rm $rtp_dir/session.c
ln -s $PWD/session.c $rtp_dir/session.c
else
ln -s $PWD/session.c $rtp_dir/session.c
fi

if [ -L $codec_dir/user_udp.h ]; then  
rm $codec_dir/user_udp.h $codec_dir/user_udp.c
ln -s $PWD/user_udp.h $codec_dir/user_udp.h
ln -s $PWD/user_udp.c $codec_dir/user_udp.c
else
ln -s $PWD/user_udp.h $codec_dir/user_udp.h
ln -s $PWD/user_udp.c $codec_dir/user_udp.c
fi

if [ -L $rtp_dir/user_udp.h ]; then  
rm $rtp_dir/user_udp.h $rtp_dir/user_udp.c
ln -s $PWD/user_udp.h $rtp_dir/user_udp.h
ln -s $PWD/user_udp.c $rtp_dir/user_udp.c
else
ln -s $PWD/user_udp.h $rtp_dir/user_udp.h
ln -s $PWD/user_udp.c $rtp_dir/user_udp.c
fi

