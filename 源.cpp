/* Copyright 2017, Gurobi Optimization, Inc. */

/* IRP

*/

#include "gurobi_c++.h"
#include<iostream>
#include<sstream>
using namespace std;

int
main(int   argc,
	char *argv[])
{
	const int Nrounds = 7;//周期是7
	const int Nretailers = 5;//零售商数量为5,0代表仓库
	double Lij[Nretailers + 1][Nretailers + 1];//从i到j的路程开销
	const int Rc = 10;//车辆的容量
	double R[Nretailers + 1];//每个零售商以及仓库的容量
	double S[Nrounds + 1][Nretailers + 1];//仓库和客户在t周期中的消费量
	double a[Nretailers + 1];//存储费率
	double W0[Nretailers + 1];//仓库以及零售商在最初的库存量
	GRBEnv *env = 0;
	GRBVar **W = 0;//Wti
	GRBVar **H = 0;//Hti
	GRBVar l[Nrounds+1][Nretailers+1][Nretailers+1];//ltij
	
	try {
		env = new GRBEnv();

		GRBModel model = GRBModel(*env);

		model.set(GRB_StringAttr_ModelName, "IRP");

		// Create variables
		for (int t = 0; t <= Nrounds; t++) {
			W[t] = model.addVars(Nretailers+1);
			for (int i = 0; i <= Nretailers; i++) {
				ostringstream vname;
				vname << "W" << t << '.' << i;
				W[t][i].set(GRB_DoubleAttr_UB, R[i]);
				W[t][i].set(GRB_DoubleAttr_LB, 0.0);//库存不为负数
				W[t][i].set(GRB_DoubleAttr_Obj, a[i]);
				W[t][i].set(GRB_StringAttr_VarName, vname.str());
			}
		}//定义Wti

		for (int t = 1; t <= Nrounds; t++)
		{
			H[t] = model.addVars(Nretailers);
			for (int i = 1; i <= Nretailers; i++) {
				ostringstream vname;
				vname << "H" << t << '.' << i;
				H[t][i].set(GRB_DoubleAttr_LB, 0.0);//放货量不为负数
				H[t][i].set(GRB_StringAttr_VarName, vname.str());
			}
		}//定义Hti

		for (int t = 1; t <= Nrounds; t++) {
			for (int i = 0; i <= Nretailers; i++) {
				for (int j = 0; j <= Nretailers; j++) {
					ostringstream vname;
					vname << 'l' << t << '.' << i << '.' << j;
					l[t][i][j] = model.addVar(0.0, 1.0, Lij[i][j], GRB_BINARY, vname.str());
				}
			}
		}//定义ltij


		// Add constraints
		for (int i = 0; i <= Nretailers; i++) {
			ostringstream vname;
			vname << "W0." << i;
			model.addConstr(W[0][i], GRB_EQUAL, W0[i],vname.str());//为W0i赋初值
		}

		for (int t = 1; t <= Nrounds; t++) {
			GRBLinExpr rhs = 0;
			for (int i = 1; i <= Nretailers; i++) {
				rhs += H[t][i];
			}
			model.addConstr(Rc, GRB_GREATER_EQUAL, rhs);//车辆不能超载
			rhs += W[t][0];
			GRBLinExpr hs = S[t][0] + W[t - 1][0];
			model.addConstr(rhs, GRB_EQUAL, hs);//仓库库存更新
		}

		for (int t = 1; t <= Nrounds; t++) {
			for (int i = 1; i <= Nretailers; i++) {
				model.addConstr(W[t][i] == W[t - 1][i] + S[t][i] + H[t][i]);//更新客户的库存
			}
		}

		for (int t = 1; t <= Nrounds; t++) {
			GRBLinExpr lhs = 0;
			GRBLinExpr rhs2 = 0;
			for (int i = 1; i <= Nretailers; i++) {
				lhs += H[t][i];
				rhs2 += l[t][0][i];
			}
			GRBLinExpr rhs1=(W[t - 1][0] - R[0] + S[t][0]);//当前周期中溢出的货物量
			model.addConstr(lhs, GRB_GREATER_EQUAL, rhs1);//总出货量要大于溢出的货物量，即仓库不能过载
			model.addConstr(lhs, GRB_GREATER_EQUAL, rhs2);//当出度>0时，出货量>0;出度==0时，出货量==0
		}

		for (int t = 1; t <= Nrounds; t++) {
			for (int i = 1; i <= Nretailers; i++) {
				GRBLinExpr rhs2 = 0;
				for (int j = 0; j <= Nretailers; j++) {
					rhs2 += Rc*l[t][i][j];
				}
				GRBLinExpr rhs1 = R[i] - (W[t - 1][i]+S[t][i]);//该周期中此零售商剩余的存储空间
				model.addConstr(H[t][i], GRB_LESS_EQUAL, rhs1);//保证客户不超载
				model.addConstr(H[t][i], GRB_LESS_EQUAL, rhs2);//当没有访问，则必然没有放货
			}
		}

		for (int t = 1; t <= Nrounds; t++) {
			GRBLinExpr lhs = 0;
			GRBLinExpr rhs = 0;
			for (int i = 1; i <= Nretailers; i++) {
				lhs += H[t][i];
				rhs += l[t][0][i]*Rc;
			}
			model.addConstr(lhs, GRB_LESS_EQUAL, rhs);//当仓库有出货量时，仓库一定有出度，当出度为零时出货量必定为0
		}

		for (int t = 1; t <= Nrounds; t++) {
			for (int i = 0; i <= Nretailers; i++) {
				GRBLinExpr lhs = 0;
				GRBLinExpr rhs = 0;
				for (int j = 0; j < Nretailers; j++) {
					lhs += l[t][j][i];
					rhs += l[t][i][j];
				}
				model.addConstr(lhs, GRB_EQUAL, rhs);//每个零售商或者仓库的入度等于出度
				model.addConstr(lhs, GRB_LESS_EQUAL, 1.0);//每个零售商或者仓库的入度小于等于1
			}
		}

		// Optimize model

		model.optimize();


		
	}
	catch (GRBException e) {
		cout << "Error code = " << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
	}
	catch (...) {
		cout << "Exception during optimization" << endl;
	}

	
	delete env;
	return 0;
}
