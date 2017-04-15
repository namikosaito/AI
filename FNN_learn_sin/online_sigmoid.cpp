#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define No 150000			//�w�K��
#define Input_size 20		//���̓f�[�^�̌�
#define Input_dim 1		//���͑w�̃j���[������
#define Hidden_dim 10		//���ԑw�̃j���[������
#define Output_dim 1            //�o�͑w�̃j���[������
#define test_size 200		//���ؗp�f�[�^�̍��ތ��i���炩���j
#define eta  0.1		//�w�K�W��
int alpha=10;       		//�V�O���C�h�֐��̌X��
int gain=2;     		//�V�O���C�h�̏o�͔͈͂��L����Q�C��

#define INPUT_x "INPUT_x.csv"
#define INPUT_y "INPUT_y.csv"
#define error_file "ERROR.csv"

void AddTarget();
void InitParameters();
void ForwardPropagation(int t);
void GetError(int t,int k);
void BackwardPropagation(int t);
void Test();

/*���́A�o�͂̋��t�f�[�^���i�[*/
double x[Input_size][Input_dim] = { 0 }, y[Input_size][Output_dim] = { 0 };
/*���͂ƒ��ԁA���ԂƏo�͂̏d��*/
double v[Hidden_dim][Input_dim + 1] = { 0 }, w[Output_dim][Hidden_dim + 1] = { 0 };
/*���́A���ԁA�o�͂̃j���[�����̒l(+1�̓o�C�A�X��)*/
double I[Input_dim + 1] = { 0 }, H[Hidden_dim + 1],O[Output_dim] = { 0 };
/*�G���[�̒l*/
double E[No] = { 0 };
/*�e�X�g����*/
double test_out[test_size][Input_dim + Output_dim];
/*���z*/
double dk[Output_dim], dj[Hidden_dim] = { 0 };
double temp;

FILE *fp2;

int main()
{
	int i, j, k , t;
	AddTarget();		//�w�K��������(���t�M��)�����
	InitParameters();	//�w�K�ɕK�v�ȕϐ��̏������ƕ\��

	fp2=fopen(error_file,"w");
	for (k = 0; k < No;k++){
		for (t = 0; t < Input_size; t++){
			ForwardPropagation(t);
			GetError(t,k);
			BackwardPropagation(t);
		}
		fprintf(fp2,"%f\n",E[k]);
		printf("%f\n",E[k]);
	}
	fclose(fp2);
	Test();
	return 0;
}

/*******************************************************/
void AddTarget(){	//�����M����ϐ��Ɋi�[
	FILE *fp;
	int i, j;
	
	fp=fopen(INPUT_x,"r");
	if (fp==NULL){
		printf("cannot open file input_x\n");
		exit(-1);
	}
	for (i = 0; i < Input_size; i++){
		for (j = 0; j < Input_dim; j++){
			fscanf(fp, "%lf ", &x[i][j]);
		}
	}
	fclose(fp);
       
	fp=fopen(INPUT_y,"r");
	if (fp==NULL){
		printf("cannot open file input_y\n");
		exit(-1);
	}
	for (i = 0; i < Input_size; i++){
	  for(j=0;j<Output_dim;j++){
		fscanf(fp, "%lf ", &y[i][j]);
	  }
	}
	fclose(fp);
}


void InitParameters(){	//�ϐ��̏�����
	int i, j;
	for (i = 0; i < Input_size; i++){
	  for(j=0;j<Output_dim;j++){
		y[i][j] = (gain/2+y[i][j])/gain; //�V�O���C�h�֐��̏o�͔͈͂��g��
	  }
	}

	I[0] = 1; //�o�C�A�X
	H[0] = 1;

	//	srand((unsigned int)time(NULL));

	for (i = 0; i < Hidden_dim; i++){	//���͂ƒ��Ԃ̌����萔�̏�����
		for (j = 0; j < Input_dim + 1; j++){
			v[i][j] = 0.2*rand() / (double)RAND_MAX - 0.1;	//����
		}
	}
	for(i=0;i<Output_dim;i++){
	        for (j = 0;j<Hidden_dim+1;j++){	//���ԂƏo�͂̌����萔�̏�����
	         	w[i][j] = 0.2*rand() / (double)RAND_MAX - 0.1;
	        }
	}
}

void ForwardPropagation(int t){		//�O�����v�Z
	int i, j;
	//�f�[�^�ɏ]���ăl�b�g���[�N�ւ̓��͐ݒ�
	for (i = 0; i < Input_dim; i++){
		I[i + 1] = x[t][i];
	}

	//�B��f�q�̌v�Z
	for (i = 0; i < Hidden_dim; i++){
		temp = 0;
		for (j = 0; j < Input_dim + 1; j++){
			temp += I[j] * v[i][j]; //�B��w�̓������
		}
		H[i + 1] = 1 / (1 + exp(-alpha*temp));	//������
	}

	temp = 0;
	for(i=0;i<Output_dim;i++){
         	for (j =0;j<Hidden_dim+1;j++){
	        	temp += H[j]* w[i][j]; //�o�͑w�̓������
	        }
		O[i] = 1 / (1 + exp(-alpha*temp));		//������
	}
}

void GetError(int t,int k){		//�G���[���Z�o
  int i;
  for(i=0;i<Output_dim;i++){
    E[k]+= 1 / 2.0*pow(gain*(y[t][i] - O[i]), 2); //�Q��덷
  }
}

void BackwardPropagation(int t){	//�������v�Z
        int i,j;
        for(i=0;i<Output_dim;i++){
	        dk[i] = -(y[t][i] - O[i])*O[i]*(1 - O[i]);
        }
	for (i = 0; i < Hidden_dim; i++){
	  for(j=0;j<Output_dim;j++){
		dj[i] = dk[j]*w[j][i + 1] * H[i + 1] * (1 - H[i + 1]);
	  }
	}
	for(i=0;i<Output_dim;i++){
	  for (j = 0;j< Hidden_dim + 1; j++){
		w[i][j] -= eta*dk[i]*H[j];
	  }
	}
	for (i = 0; i < Hidden_dim; i++){
		for (j = 0; j < Input_dim + 1; j++){
			v[i][j] -= eta*dj[i] * I[j];
		}
	}
}

void Test(){	//���ۂɃv���b�g���Ă���
	int i, j, t;
	double max[Input_dim] = { 0 }, min[Input_dim] = { 0 };
	double p;
	double test[test_size + 1][Input_dim+Output_dim];
	FILE *fp_x,*fp_y;
	fp_x=fopen("OUTPUT_x.csv","w");
	fp_y=fopen("OUTPUT_y.csv","w");
	
	if (fp_x==NULL||fp_y==NULL){
		printf("cannot open file\n");
		exit(-1);
	}

	for (i = 0; i < Input_dim; i++){	//max��min�̏�����
		max[i] = x[0][i];
		min[i] = x[0][i];
	}

	for (i = 0; i < Input_size; i++){
		for (j = 0; j < Input_dim; j++){
			if (max[j] < x[i][j]){
				max[j] = x[i][j];
			}
			if (min[j] > x[i][j]){
				min[j] = x[i][j];
			}
		}
	}

	for (i = 0; i < Input_dim; i++){
		j = 0;
		for (p = min[0]; p <= max[0]; p += (max[0] - min[0]) / test_size){
			test[j][i] = p;
			j++;
		}
	}

	for (t = 0; t < test_size + 1; t++){
		for (i = 0; i < Input_dim; i++){
			I[i + 1] = test[t][i];
		}

		for (i = 0; i < Hidden_dim; i++){
			temp = 0;
			for (j = 0; j < Input_dim + 1; j++){
				temp += I[j] * v[i][j];
			}
			H[i + 1] = 1 / (1 + exp(-alpha*temp));	
		}

		temp = 0;
		for(i=0;i<Output_dim;i++){
		    for (j = 0; j < Hidden_dim + 1; j++){
			temp += H[j] * w[i][j];
		    }
		    O[i]=1/(1+exp(-alpha*temp));
		}     

		for (i = 0; i < Input_dim + Output_dim; i++){
		        if (i < Input_dim){
		            test_out[t][i] = test[t][i]; //x
			    fprintf(fp_x,"%f",test_out[t][i]);
			}
			else{
			    test_out[t][i] = O[Output_dim-Input_dim]*gain - gain / 2.0; //y
			    fprintf(fp_y,"%f",test_out[t][i]);
			}
			if(i==Input_dim-1)fprintf(fp_x,"\n");
			if(i==Input_dim+Output_dim-1)fprintf(fp_y,"\n");
		}
	}
	fclose(fp_x);
	fclose(fp_y);
}

