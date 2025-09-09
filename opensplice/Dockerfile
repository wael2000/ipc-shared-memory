# syntax=docker/dockerfile:1.7
FROM registry.access.redhat.com/ubi9/ubi-minimal:9.4

ARG APP_NAME=my-dds-app
ARG OSPL_DIR=/opt/opensplice
ARG APP_DIR=/opt/app

# Install minimal runtime deps
RUN microdnf -y install shadow-utils ca-certificates iproute && microdnf -y clean all

# --- OpenSplice runtime (provide your own artifact) ---
# e.g., COPY dist/OPEN_SPLICE_DIST.tgz /tmp/
# RUN mkdir -p ${OSPL_DIR} && tar -xzf /tmp/OPEN_SPLICE_DIST.tgz -C ${OSPL_DIR} --strip-components=1
# If you already have a flattened runtime dir, just copy it:
# COPY dist/opensplice-runtime/ ${OSPL_DIR}/

# --- Application bits ---
# Expect the app binary + any libs staged by CI
#   bin/${APP_NAME}      -> main executable linking to OpenSplice libs
#   lib/*                -> optional rpath/shared libs
COPY build/bin/ ${APP_DIR}/bin/
COPY build/lib/ ${APP_DIR}/lib/

# Create a writable var dir for OpenSplice logs/db
RUN mkdir -p ${OSPL_DIR}/var/log ${OSPL_DIR}/var/run ${OSPL_DIR}/var/db \
    && mkdir -p ${APP_DIR}/config

# Add entrypoint and make it executable
COPY container/entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod 0755 /usr/local/bin/entrypoint.sh

# OpenShift: run as arbitrary UID; ensure world-writable where needed
# (Avoid chown here; random UID cannot change ownership at build time)
RUN chmod -R g=u,o=u ${OSPL_DIR} ${APP_DIR}

# Do not set USER to a fixed uid; let OpenShift inject a random uid.
# Expose typical DDSI2/RTPS discovery/data ports (not strictly needed for ClusterIP)
EXPOSE 7400-7500/udp

ENV OSPL_HOME=${OSPL_DIR} \
    OSPL_URI=file://${APP_DIR}/config/ospl.xml \
    OSPL_INFOFILE=${OSPL_DIR}/var/log/ospl-info.log \
    OSPL_ERRORFILE=${OSPL_DIR}/var/log/ospl-error.log \
    LD_LIBRARY_PATH=${APP_DIR}/lib:${OSPL_DIR}/lib:${LD_LIBRARY_PATH} \
    APP_BIN=${APP_DIR}/bin/${APP_NAME}

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["run"]
