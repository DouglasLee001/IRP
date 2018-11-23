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

| Variable     | Description                | Type      | Domain | Remark                |
| ------------ | -------------------------- | --------- | ------ | --------------------- |
| $H^{t}_{i}$  | 在t周期车辆在客户i的放货量 | $integer$ |        | $$i\in I\quad,i\ne0$$ |
| $l^{t}_{ij}$ | 在t周期车辆是否使用路线ij  | $bool$    | {0,1}  |                       |
| $W^t_{i}$    | 在t周期中i中的库存         | $integer$ |        |                       |

## objective

$$min\sum_{j}\sum_{i}\sum_{t}(L^{t}_{ij}*l^t_{ij})+\sum_{i}\sum_{t}(a^{i}*W^{t}_{i})$$

$$i,j\in I,i \ne j,t\in T$$

## Constraints

###车辆不能超载..

$$
R\ge\sum_{i}H^{t}_{i},\quad\forall t\in T
$$

### 客户&仓库的库存不为负..

$$
W^{t}_{i}\ge0\quad,i\in I\quad,\forall t\in T
$$

### 客户的库存更新..

$$
W^{t}_{i}=W^{t-1}_{i}+S^{t}_{i}+H^{t}_{i}\quad,i\in I\quad,i\ne0\quad,t\in T
$$

### 仓库的库存更新..

$$
W^{t}_{0}=S^{t}_{0}-\sum_{i}H^{t}_{i}+W^{t-1}_{0}\quad,t\in T
$$

### 仓库不能过载，并且当仓库没有出货量的时候，出度必然为0（若没有出货量且出度不为0，则必然亏损路费）..

$$
\sum_{i}H^{t}_{i}\ge (W^{t-1}_{0}-R_{0}+S^{t}_0)*\sum_il^t_{0i}\quad,\forall t\in T
$$

##### ==上述式子存在一个问题，如果$(W^{t-1}_{0}-R_{0}+S^{t}_0)>0$并且仓库出度和出货量都为0，那么约束成立，但是不符合条件；此处可以将其线性化表示为如下==

$$
\sum_{i}H^{t}_{i}\ge (W^{t-1}_{0}-R_{0}+S^{t}_0) ,  \sum_{i}H^{t}_{i}\ge\sum_il^t_{0i}\quad,\forall t\in T
$$



### 每个顾客不能过载，同时如果没有访问，则必然没有放货，即$H^{t}_{i}=0$。（这里可能出现没有放货但是访问的情况，这条约束或许可以加上）



$$
H^{t}_{i}\le \sum_{j} l^t_{ij}*(R_i-(W^{t-1}_i))
$$

##### ==此处可以将其线性化表示为，注意这里的是先消耗再放货==

$$
H^t_i\le R_i-(W^{t-1}_i)  ,  H^t_i\le\sum_{j} l^t_{ij}*R,j\in I
$$



### 当仓库有出货量时，仓库一定有出度，当出度为零时出货量必定为0..

$$
\sum_{i}H^{t}_{i}\le inf*\sum_i l^{t}_{0i}
$$

##### ==PS:此处可以进行修改，将$inf$改为$R$==

$$
\sum_{i}H^{t}_{i}\le R*\sum_i l^{t}_{0i}
$$

### 当仓库或客户有出度时，必有入度，反之亦然..

$$
\sum_{i}l^{t}_{ji}=\sum_{i}l^{t}_{ij}\quad,i,j\in I
$$

### 不能出现环，每个结点的入度最大为1(或可删去)..

$$
\sum_{i}l^{t}_{ji}\le1,i\in I
$$

### 防止出现子回路的两种方案

#### 添加决策变量$U^t_{i}$

$$
U^t_i-U^t_j+N*L^l_{ij}\le (N-1),
 0\le U^t_i\le(N-1)，N=零售商数量
$$

#### 添加回调函数

$$
\sum _i\sum_jl^t{ij}\le N-1,i.j\in子回路，N=子回路长度
$$

