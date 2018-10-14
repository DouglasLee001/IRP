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
	const int Nrounds = 7;//������7
	const int Nretailers = 5;//����������Ϊ5,0����ֿ�
	double Lij[Nretailers + 1][Nretailers + 1];//��i��j��·�̿���
	const int Rc = 10;//����������
	double R[Nretailers + 1];//ÿ���������Լ��ֿ������
	double S[Nrounds + 1][Nretailers + 1];//�ֿ�Ϳͻ���t�����е�������
	double a[Nretailers + 1];//�洢����
	double W0[Nretailers + 1];//�ֿ��Լ�������������Ŀ����
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
				W[t][i].set(GRB_DoubleAttr_LB, 0.0);//��治Ϊ����
				W[t][i].set(GRB_DoubleAttr_Obj, a[i]);
				W[t][i].set(GRB_StringAttr_VarName, vname.str());
			}
		}//����Wti

		for (int t = 1; t <= Nrounds; t++)
		{
			H[t] = model.addVars(Nretailers);
			for (int i = 1; i <= Nretailers; i++) {
				ostringstream vname;
				vname << "H" << t << '.' << i;
				H[t][i].set(GRB_DoubleAttr_LB, 0.0);//�Ż�����Ϊ����
				H[t][i].set(GRB_StringAttr_VarName, vname.str());
			}
		}//����Hti

		for (int t = 1; t <= Nrounds; t++) {
			for (int i = 0; i <= Nretailers; i++) {
				for (int j = 0; j <= Nretailers; j++) {
					ostringstream vname;
					vname << 'l' << t << '.' << i << '.' << j;
					l[t][i][j] = model.addVar(0.0, 1.0, Lij[i][j], GRB_BINARY, vname.str());
				}
			}
		}//����ltij


		// Add constraints
		for (int i = 0; i <= Nretailers; i++) {
			ostringstream vname;
			vname << "W0." << i;
			model.addConstr(W[0][i], GRB_EQUAL, W0[i],vname.str());//ΪW0i����ֵ
		}

		for (int t = 1; t <= Nrounds; t++) {
			GRBLinExpr rhs = 0;
			for (int i = 1; i <= Nretailers; i++) {
				rhs += H[t][i];
			}
			model.addConstr(Rc, GRB_GREATER_EQUAL, rhs);//�������ܳ���
			rhs += W[t][0];
			GRBLinExpr hs = S[t][0] + W[t - 1][0];
			model.addConstr(rhs, GRB_EQUAL, hs);//�ֿ������
		}

		for (int t = 1; t <= Nrounds; t++) {
			for (int i = 1; i <= Nretailers; i++) {
				model.addConstr(W[t][i] == W[t - 1][i] + S[t][i] + H[t][i]);//���¿ͻ��Ŀ��
			}
		}

		for (int t = 1; t <= Nrounds; t++) {
			GRBLinExpr lhs = 0;
			GRBLinExpr rhs2 = 0;
			for (int i = 1; i <= Nretailers; i++) {
				lhs += H[t][i];
				rhs2 += l[t][0][i];
			}
			GRBLinExpr rhs1=(W[t - 1][0] - R[0] + S[t][0]);//��ǰ����������Ļ�����
			model.addConstr(lhs, GRB_GREATER_EQUAL, rhs1);//�ܳ�����Ҫ��������Ļ����������ֿⲻ�ܹ���
			model.addConstr(lhs, GRB_GREATER_EQUAL, rhs2);//������>0ʱ��������>0;����==0ʱ��������==0
		}

		for (int t = 1; t <= Nrounds; t++) {
			for (int i = 1; i <= Nretailers; i++) {
				GRBLinExpr rhs2 = 0;
				for (int j = 0; j <= Nretailers; j++) {
					rhs2 += Rc*l[t][i][j];
				}
				GRBLinExpr rhs1 = R[i] - (W[t - 1][i]+S[t][i]);//�������д�������ʣ��Ĵ洢�ռ�
				model.addConstr(H[t][i], GRB_LESS_EQUAL, rhs1);//��֤�ͻ�������
				model.addConstr(H[t][i], GRB_LESS_EQUAL, rhs2);//��û�з��ʣ����Ȼû�зŻ�
			}
		}

		for (int t = 1; t <= Nrounds; t++) {
			GRBLinExpr lhs = 0;
			GRBLinExpr rhs = 0;
			for (int i = 1; i <= Nretailers; i++) {
				lhs += H[t][i];
				rhs += l[t][0][i]*Rc;
			}
			model.addConstr(lhs, GRB_LESS_EQUAL, rhs);//���ֿ��г�����ʱ���ֿ�һ���г��ȣ�������Ϊ��ʱ�������ض�Ϊ0
		}

		for (int t = 1; t <= Nrounds; t++) {
			for (int i = 0; i <= Nretailers; i++) {
				GRBLinExpr lhs = 0;
				GRBLinExpr rhs = 0;
				for (int j = 0; j < Nretailers; j++) {
					lhs += l[t][j][i];
					rhs += l[t][i][j];
				}
				model.addConstr(lhs, GRB_EQUAL, rhs);//ÿ�������̻��ֿ߲����ȵ��ڳ���
				model.addConstr(lhs, GRB_LESS_EQUAL, 1.0);//ÿ�������̻��ֿ߲�����С�ڵ���1
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
