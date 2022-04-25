FROM ubuntu:impish

RUN ln -fs /usr/share/zoneinfo/Europe/Madrid /etc/localtime

RUN set -ex; apt update; \
	apt upgrade -y; \
	apt install -y --no-install-recommends \
	cmake \
	npm \
	libv8-dev \
	build-essential \
	gcc \
	g++

RUN set -ex; npm install -g typescript

RUN groupadd --gid 1000 user \
  && useradd --uid 1000 --gid user --shell /bin/bash --create-home user

COPY . ./cpp-v8-typescript-example

RUN mkdir build-cpp-v8-typescript-example-Release

RUN cd build-cpp-v8-typescript-example-Release && \
	cmake ../cpp-v8-typescript-example -DCMAKE_BUILD_TYPE=Release && \
	cmake --build . --target all

CMD ["/bin/bash"]
