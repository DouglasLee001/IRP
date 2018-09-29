[TOC]

# IRP



## known

### sets

| Set  | Description        | Size      | Element | Remark                 |
| :--- | ------------------ | --------- | ------- | ---------------------- |
| $I$  | retailers&producer | $[0, 30]$ | $i,j$   | 0 means producer       |
| $T$  | 运货周期           | $[0, 7]$  | $t$     | 0 means original state |

### constants

| Constant    | Description            | Type    | Range     | Remark                 |
| ----------- | ---------------------- | ------- | --------- | ---------------------- |
| $W^0_{i}$   | initial inventory      | integer | $[1,200]$ | i$$ \in I $$           |
| $L_{ij}$    | cost between retailers | integer |           | $$i,j \in I$$ 对称矩阵 |
| $a_{i}$     | 存储费率               | real    | $(0,1]$   | $$i \in I$$            |
| $R_{i}$     | 客户与仓库的容量       | integer |           | $$i \in I$$            |
| $R$         | 车的容量               | integer |           |                        |
| $S^{t}_{0}$ | 仓库每周期生产的量     | integer | $(0,300)$ | $$t \in T$$            |
| $S^{t}_{i}$ | 客户各个周期消费量     | integer | $(-20,0)$ | $$t \in T,i \in I$$    |

## decision

| Variable     | Description                | Type      | Domain | Remark                 |
| ------------ | -------------------------- | --------- | ------ | ---------------------- |
| $F^{t}_{i}$  | 在t周期中车辆是否访问客户i | $bool$    | {0,1}  |                        |
| $l^{t}_{ij}$ | 在t周期车辆是否使用路线ij  | $bool$    | {0,1}  |                        |
| $H^{t}_{i}$  | 在t周期车辆在客户i放的货量 | $integer$ |        | $$ i\in I\quad,i\ne0$$ |

## objective

$$min\sum_{j}\sum_{i}\sum_{t}(L^{t}_{ij}*l^t_{ij})+\sum_{i}\sum_{t}(a^{i}*W^{t}_{i})$$

$$i,j\in I,i \ne j,t\in T$$

## Constraints

###车辆不能超载

$$
R\ge\sum_{i}H^{t}_{i},\quad\forall t\in T
$$

### 客户&仓库的库存不为负

$$
W^{t}_{i}\ge0\quad,i\in I\quad,\forall t\in T
$$

### 客户的库存更新

$$
W^{t}_{i}=W^{t-1}_{i}+S^{t}_{i}+H^{t}_{i}\quad,i\in I\quad,i\ne0\quad,t\in T
$$

### 仓库的库存更新

$$
W^{t}_{0}=S^{t}_{0}-\sum_{i}H^{t}_{i}\quad,t\in T
$$

### 仓库不能过载

$$
R_{0}\ge W^{t-1}_{0}-\sum_{i}H^{t}_{i}+S^{t}\quad,\forall t\in T
$$

### 每个顾客不能过载，同时如果没有访问，$H^{t}_{i}=0$

$$
H^{t}_{i}\le F^{t}_{i}*(R_i-W^{t-1}_i)
$$

### 当仓库有出度时，才有出货量（即仅当出货总量大于零时仓库的出度为1，否则为零）

$$
\sum_{i}H^{t}_{i}\le inf*\sum_i l^{t}_{0i}
$$

$$
\sum_i l^{t}_i\le inf*\sum_{i}H^{t}_{0i}
$$

###如果客户被访问，则必有入度

$$
\sum_i l^{t}_{ij}=F^{t}_{i}\quad,i\in I\quad,i\ne0
$$

### 当仓库或客户有出度时，必有入度，反之亦然

$$
\sum_{i}l^{t}_{0i}=\sum_{i}l^{t}_{i0}\quad,i\in I
$$

