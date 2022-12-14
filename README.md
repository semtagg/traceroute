# Утилита traceroute

## Как установить?
Необходимо склонировать репозиторий, а затем в корневой папке выполнить следующую команду:
```
$ make
```

## Примеры использования
* C отображением доменного имени:
```
semtagg@semtagg:~/vscode/traceroute$ sudo ./traceroute ya.ru
traceroute to ya.ru (87.250.250.242), 30 hops max
1. _gateway (192.168.0.1)  3.091 ms  10.522 ms  10.532 ms 
2. vlan814.pi.bb.pu.ru (92.42.26.1)  2.260 ms  2.714 ms  2.718 ms 
3. 172.24.31.5 (172.24.31.5)  2.779 ms  9.749 ms  9.758 ms 
4. vunk-punk.rtr.pu.ru (172.24.25.32)  2.938 ms  3.699 ms  3.703 ms 
5. magma-vunk.rtr.pu.ru (172.24.25.38)  81.687 ms  82.165 ms  82.169 ms 
6. vlan3.kronos.pu.ru (195.70.196.3)  3.528 ms  8.001 ms  8.009 ms 
7. 195.70.206.129 (195.70.206.129)  3.689 ms  12.350 ms  12.369 ms 
8. yandex.spb.piter-ix.net (185.1.152.57)  3.668 ms  4.177 ms  4.183 ms 
9. * * * 
10. ya.ru (87.250.250.242)  16.984 ms  17.650 ms  17.659 ms
```
* Без отображения доменного имени (с флагом -n):
```
semtagg@semtagg:~/vscode/traceroute$ sudo ./traceroute -n ya.ru
traceroute to ya.ru (87.250.250.242), 30 hops max
1. 192.168.0.1  4.151 ms  4.175 ms  4.181 ms 
2. 92.42.26.1  3.892 ms  3.913 ms  3.918 ms 
3. 172.24.31.5  3.860 ms  3.882 ms  3.886 ms 
4. 172.24.25.32  3.863 ms  3.886 ms  3.889 ms 
5. 172.24.25.38  4.150 ms  4.198 ms  4.201 ms 
6. 195.70.196.3  63.740 ms  63.765 ms  63.770 ms 
7. 195.70.206.129  3.935 ms  3.956 ms  3.960 ms 
8. 185.1.152.57  4.057 ms  4.079 ms  4.083 ms 
9. * * * 
10. 87.250.250.242  15.531 ms  15.564 ms  15.570 ms
```