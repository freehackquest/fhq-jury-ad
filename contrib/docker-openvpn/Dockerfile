FROM debian:10
# Basic dockerfile copyed from here: https://github.com/dockovpn/docker-openvpn

LABEL maintainer="Evgenii Sopov <mrseakg@gmail.com>"

# ctf01d-openvpn

COPY scripts .
COPY config ./config

RUN apk add --no-cache openvpn easy-rsa bash netcat-openbsd zip dumb-init && \
    mkdir -p /opt/ctf01d-openvpn && \
    cd /opt/ctf01d-openvpn && \
    /usr/share/easy-rsa/easyrsa init-pki && \
    /usr/share/easy-rsa/easyrsa gen-dh && \
    # DH parameters of size 2048 created at /usr/share/easy-rsa/pki/dh.pem
    # Copy DH file
    cp pki/dh.pem /etc/openvpn && \
    # Copy FROM ./scripts/server/conf TO /etc/openvpn/server.conf in DockerFile
    cd /opt/ctf01d-openvpn && \
    cp config/server.conf /etc/openvpn/server.conf


EXPOSE 1194/udp
EXPOSE 8080/tcp

VOLUME [ "/opt/ctf01d-openvpn" ]

ENTRYPOINT [ "dumb-init", "./start.sh" ]
CMD [ "" ]