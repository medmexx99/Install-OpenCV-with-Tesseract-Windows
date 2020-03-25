#!/bin/bash
function MAJGitRepo
{
if [  ! -d "$myRepo/$1"  ]; then
  echo "clonning ${1}"
  git clone $2
  mkdir Build/$1
else
  echo "update $1"
  cd $1
  git pull --rebase
  cd ..
fi
}

echo "Installing leptonica"
myRepo=$(pwd)
CMAKE_CONFIG_GENERATOR="Visual Studio 15 2017 Win64"
RepoSource=nana
MAJGitRepo $RepoSource https://github.com/cnjinhao/nana.git
cd $RepoSource
git reset --hard v1.7.2
cd ..

mkdir Build
mkdir Build/$RepoSource
pushd Build/$RepoSource
cmake -G"$CMAKE_CONFIG_GENERATOR" -DCMAKE_INSTALL_PREFIX="$myRepo"/Install/nana "$myRepo/$RepoSource" -DCMAKE_DEBUG_POSTFIX:STRING="d"
#-DNANA_CMAKE_ENABLE_JPEG:BOOL=ON -DNANA_CMAKE_ENABLE_PNG:BOOL=ON -DNANA_CMAKE_LIBJPEG_FROM_OS:BOOL=OFF -DNANA_CMAKE_LIBPNG_FROM_OS:BOOL=OFF \
#-DZLIB_INCLUDE_DIR:PATH="$myRepo"/Install/zlib/include -DZLIB_LIBRARY_DEBUG:FILE="$myRepo"/Install/zlib/lib/zlibstaticd.lib -DZLIB_LIBRARY_RELEASE:FILE="$myRepo"/Install/zlib/lib/zlibstatic.lib \
#-DPNG_INCLUDE_DIR:PATH="$myRepo"/Install/lpng/include -DPNG_LIBRARY_DEBUG:FILE="$myRepo"/Install/lpng/lib/libpng16_staticd.lib -DPNG_LIBRARY_RELEASE:FILE="$myRepo"/Install/lpng/lib/libpng16_static.lib \
#-DTIFF_INCLUDE_DIR:PATH="$myRepo"/opencv/3rdparty/libtiff -DTIFF_LIBRARY_DEBUG:FILE="$myRepo"/Build/opencv/3rdparty/lib/Debug/libtiffd.lib -DTIFF_LIBRARY_RELEASE:FILE="$myRepo"/Build/opencv/3rdparty/lib/Release/libtiff.lib \
#-DJPEG_INCLUDE_DIR:PATH="$myRepo"/opencv/3rdparty/libjpeg -DJPEG_LIBRARY_DEBUG:FILE="$myRepo"/Build/opencv/3rdparty/lib/Debug/libjpeg-turbod.lib -DJPEG_LIBRARY_RELEASE:FILE="$myRepo"/Build/opencv/3rdparty/lib/Release/libjpeg-turbo.lib
echo "************************* $Source_DIR -->debug"
cmake --build .  --config release
cmake --build .  --target install --config release
cmake --build .  --config debug
cmake --build .  --target install --config debug
popd