#!/bin/bash

# Constants
image_prefix=coinxp/coinxp
version=hub_test
USERNAME=coinxp
PASSWORD=ywRnttcEUY,GPRaosBi4Un?QUooFiNcA
ORGANIZATION=coinxp

REPOSITORY=myrepo
TAG=latest

service_name=nodeosd

echo "---- delete docker image to have a clean build"
curl -u ${USERNAME}:${PASSWORD} -X "DELETE" https://cloud.docker.com/v2/repositories/${ORGANIZATION}/${ORGANIZATION}-${service_name}/tags/${version}/

echo "---- Docker build and push service ${service_name} ----"
image_name=${image_prefix}-${service_name}
docker_file=Dockerfile
sudo docker build . -f ${docker_file} --no-cache --build-arg cxpbranch=${version} --tag ${image_name}:${version}
sudo docker push ${image_name}:${version}
