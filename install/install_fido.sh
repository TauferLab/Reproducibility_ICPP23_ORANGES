#!/usr/bin/env bash

build_dir=build
project_root=$(pwd)

rm -rf ${build_dir}
mkdir -p ${build_dir}
cd ${build_dir}

#kokkos_path=$1
#kokkos_kernels_path=$2

export CC=mpicc
export CXX=mpicxx
cmake \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_CXX_COMPILER=mpicxx \
  -DCMAKE_PREFIX_PATH=${project_root} \
  -DKokkos_DIR=${project_root}/submodules/kokkos/${build_dir}/lib64/cmake/Kokkos \
  -DKokkosKernels_DIR=${project_root}/submodules/kokkos-kernels/${build_dir}/lib64/cmake/KokkosKernels \
  ..
make

cd ..
