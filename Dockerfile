FROM emscripten/emsdk:latest AS builder

RUN python3 -m pip install jinja2

WORKDIR /app

COPY CMakeLists.txt .
COPY src ./src
COPY extern ./extern
COPY assets ./assets
COPY web/index.html ./web/index.html 

RUN mkdir build_web

WORKDIR /app/build_web
RUN emcmake cmake .. -DIS_WEB=ON -DCMAKE_BUILD_TYPE=Release

RUN emmake make

FROM python:3.9-alpine

WORKDIR /server


COPY --from=builder /app/build_web/black_hole_simulator.html ./index.html
COPY --from=builder /app/build_web/black_hole_simulator.js ./
COPY --from=builder /app/build_web/black_hole_simulator.wasm ./
COPY --from=builder /app/build_web/black_hole_simulator.data ./

EXPOSE 8000

CMD ["python3", "-m", "http.server", "8000"]
