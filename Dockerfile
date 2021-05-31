# Dockerfile for building imguiwrap in a Linux environment. See also
# docker-build.sh

FROM ubuntu:latest AS base-os

# If you have a local cache or proxy for apt packages; see apt-cacher-ng.
# e.g: APT_CACHE=http://apt-cache.local:3142/
ARG APT_CACHE

RUN \
	if [ -z "${APT_CACHE}"; then echo "Acquire::http::Proxy \"${APT_CACHE}\";" >/etc/apt/apt.conf.d/02proxy; fi; \
	echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections ; \
	mkdir /src

RUN apt update	&& \
	apt install -qy --upgrade apt-transport-https apt-utils && \
	apt upgrade -qy && \
	apt install -qy --upgrade \
        build-essential \
        cmake ninja-build cmake-doc  \
		clang-11 clang-tidy-11 clang-format-11 && \
	apt autoremove -qy && apt clean auto && rm -rf /var/lib/apt/lists/*

FROM base-os AS imgui-packages

RUN apt update && \
	apt install -qy \
		libx11-dev \
		libsdl2-dev \
		libglfw3-dev && \
	apt autoremove -qy && apt clean auto && rm -rf /var/lib/apt/lists/*

FROM imgui-packages

# Mount this folder as /src, e.g.
# docker --rm -it -v ${pwd}:/src kfsone/imguibuild
VOLUME /src

ENTRYPOINT ["/src/docker-build.sh"]
