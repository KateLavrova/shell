#include <iostream>
#include <cstring>
using namespace std;

class Rectangle
{
	public:
		int x1; int y1;	int x2;	int y2;

		Rectangle(int x, int y, int a, int b)
		{
			x1=x;y1=y;x2=a;y2=b;
		}

		void PrintRect(Rectangle Rect)
		{
			cout<<"1-ая координата: ("<<x1<<","<<y1<<"); "<<"2-ая координата: ("<<x2<<","<<y2<<")"<<endl;
		}
		
		void MoveRect(Rectangle r1)
		{
			int x; int y;
			cout<<"Введите расстояния по x и y:";
			cin>>x>>y;
			x1+=x; x2+=x; y1+=y; y2+=y;
		}

		void ChangeRect(Rectangle r1)
		{
			cout<<"Введите длину и ширину:";
			int x; int y;
			cin>>x>>y;
			x2+=x; y2+=y;
		}

		void MinRect(Rectangle r1, Rectangle r2)
		{
			int a1 = min(r1.x1,r2.x1);
			int a2 = min(r1.x2,r2.x2);
			int a = min(a1,a2);
			int b1 = max(r1.x1,r2.x1);
			int b2 = max(r1.x2,r2.x2);
			int b = max(b1,b2);
			int c1 = min(r1.y1,r2.y1);
			int c2 = min(r1.y2,r2.y2);
			int c = min(c1,c2);
			int d1 = max(r1.y1,r2.y1);
			int d2 = max(r1.y2,r2.y2);
			int d = max(d1,d2);
			Rectangle r3(a,c,b,d);
			r3.PrintRect(r3);
		}

		void CrossRect(Rectangle r1, Rectangle r2)
		{
			int a1 = min(r1.x1,r2.x1);
			int a2 = min(r1.x2,r2.x2);
			int a = min(a1,a2);
			int b1 = max(r1.x1,r2.x1);
			int b2 = max(r1.x2,r2.x2);
			int b = max(b1,b2);
			int c1 = min(r1.y1,r2.y1);
			int c2 = min(r1.y2,r2.y2);
			int c = min(c1,c2);
			int d1 = max(r1.y1,r2.y1);
			int d2 = max(r1.y2,r2.y2);
			int d = max(d1,d2);
			int k[4] = {r1.x1,r1.x2,r2.x1,r2.x2};
			int l[4] = {r1.y1,r1.y2,r2.y1,r2.y2};
			int a3,a4,b3,b4;
			for(int i=0; i<4; i++)
			{	
				if((k[i]!=b)&&(k[i]!=a))
				a3 = k[i];
				if((l[i]!=d)&&(l[i]!=c))
				b3 = l[i];
			}
			for (int i =0;i<4;i++)
			{
				if((k[i]!=b) && (k[i]!=a) && (k[i]!=a3))
				a4 = k[i];
				if((l[i]!=d) && (l[i]!=c) && (l[i]!=b3))
				b4 = l[i];
			}
			Rectangle r3(a3,b3,a4,b4);
			r3.PrintRect(r3);
		}
};

void menu()
{
	cout<<"Вывести координаты прямоугольников: see"<< endl;
	cout<<"Переместить прямоугольник на плоскости: move"<<endl;
	cout<<"Изменить размер прямоугольника: change"<<endl;
	cout<<"Построить наименьший прямоугольник, содержащий 2 заданных: union"<<endl;
	cout<<"Построить прямоугольник, являющийся пересечением: cross"<<endl;
	cout<<"------------------------------------------------------------------"<<endl;
}

int main()
{
	int x1,y1,a1,b1,x2,y2,a2,b2;
	menu();
	cout<<"Введите координаты 1-го прямоугольника: ";
	cin>>x1>>y1>>a1>>b1;
	cout<<"Введите координаты 2-го прямоугольника: ";
	cin>>x2>>y2>>a2>>b2;	
	Rectangle r1(x1,y1,a1,b1);
	Rectangle r2(x2,y2,a2,b2);
	r1.PrintRect(r1);
	r2.PrintRect(r2);
	char str[256];
	while(1)
	{
		cin>>str;
		if (!strcmp("see", str)) 
		{
			r1.PrintRect(r1); 
			r2.PrintRect(r2);
		}
		else if (!strcmp("move1", str)) r1.MoveRect(r1);
		else if (!strcmp("move2", str)) r2.MoveRect(r2);
		else if (!strcmp("change1", str)) r1.ChangeRect(r1);
		else if (!strcmp("change2", str)) r2.ChangeRect(r2);
		else if (!strcmp("union", str)) r1.MinRect(r1,r2);
		else if (!strcmp("cross", str)) r1.CrossRect(r1,r2);
		else if (!strcmp("end", str)) break;
		else cout<<"ERROR"<<endl;
	}
	return 0;
}
