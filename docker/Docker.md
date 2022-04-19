## Commands for local use of the docker environment.

Build docker image

'DOCKER_BUILDKIT=1 docker build . -t iseaframe_cicd:latest \
--secret id=MATLAB_LICENSE_KEY,src=secrets/MATLAB_LICENSE_KEY.txt \
--secret id=MATLAB_NETWORK_LICENSE,src=secrets/MATLAB_NETWORK_LICENSE.txt \
--secret id=MATLAB_UNIX_NETRC,src=secrets/MATLAB_UNIX_NETRC.txt \
--secret id=MATLAB_WIN_NETRC,src=secrets/MATLAB_WIN_NETRC.txt \
--secret id=FRAMEWORKEXTENSIONS_NETRC,src=secrets/FRAMEWORKEXTENSIONS_NETRC.txt \
--progress=plain'

Build local docker image

'DOCKER_BUILDKIT=1 docker build . -t iseaframe_cicd:local -f Dockerfile.local'

Run local docker image

'docker run --rm -it iseaframe_cicd:local'

