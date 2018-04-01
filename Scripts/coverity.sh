#!/bin/bash

cd "$(dirname "$0")"
cd ..

TMP_COMPILE_FILE=".tmp.compile.sh"
echo -e "#!/bin/bash \n\n mkdir -p build \n cd build \n cmake .. \n make -j6" > ${TMP_COMPILE_FILE}
chmod +x ${TMP_COMPILE_FILE}
cov-build --dir cov-int ./${TMP_COMPILE_FILE}
rm ${TMP_COMPILE_FILE}
rm -rf cov-int
tar -czvf Bang.tgz cov-int
