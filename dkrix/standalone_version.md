
## 


1. 

 https://pan.baidu.com/s/1-DufSEmnydMbOtTwOo_h8A  6bcl

 Mediafire https://www.mediafire.com/file/017bif66kyieviw/DARKEDEN.zip/file

2.  `DARKEDEN/Data/Info/GameClient.inf`  `127.0.0.1`

3. DARKEDEN  bat  ''  '' 

## 

 Docker Desktop

https://docs.docker.com/desktop/install/windows-install/


 docker 


```
docker pull tiancaiamao/darkeden
```

 mysql  docker 


```
docker pull mysql/mysql-server:5.7
```

 docker-compose.yml 


```
version: '3'

services:

  odk-mysql:
    image: mysql/mysql-server:5.7
    volumes:
      - ../initdb:/docker-entrypoint-initdb.d/
    environment:
        MYSQL_ROOT_PASSWORD: 123456
    restart: on-failure
    command: mysqld --sql_mode="ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION"
    networks:
      - odk-network

  odk-server:
    image: tiancaiamao/darkeden:latest
    build:
      context: ..
      dockerfile: Dockerfile
    command: ["sleep","infinity"]
    ports:
      - "9999:9999"
      - "9998:9998"
      - "9997:9997"
      - "9997:9997/udp"
    networks:
      - odk-network

networks:
  odk-network:
```


 docker 


```
docker-compose -f docker-compose.yml up -d
```



```
docker exec -it docker_odk-server_1  ./start.sh
```




```sh
docker exec -it docker_odk-server_1  ./stop.sh
docker-compose down
```
