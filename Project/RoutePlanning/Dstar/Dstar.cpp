#include "Dstar.h"
using namespace std;

D_star::D_star()
{
	// 初始化
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			tuple<int, int> t(i, j);
			double d = sqrt(i * i + j * j);
			this->Alldirec.insert(make_pair(t, d));
		}
	}
	this->b.clear();
	this->OPEN.clear();
	this->h.clear();
	this->tag.clear();
	this->path.clear();

	start = make_tuple(4, 4);  // 这里是起始点
	goal = make_tuple(16, 16); // 这里是终止点

	obs_r = 2;                    // 初始障碍球半径
	obs_pos = make_tuple(10, 10);   // 初始障碍球位置

	count = 0;
}



//D_star;; int road()
//{
//	/*获取障碍点坐标*/
//
//	int I[10][2] = { 0 };
//	ifstream myfile("road.txt");
//	LOCATE loc[10];
//	//打开并读取road.txt
//	if (!myfile.is_open())
//	{
//		cout << "can not open this file" << endl;
//		return 0;
//	}
//	for (int i = 0; i < 10; i++)
//	{
//		for (int j = 0; j < 2; j++)
//		{
//			myfile >> I[i][j];
//		}
//	}
//	for (int i = 0; i < 10; i++)
//	{
//		loc[i].x_ = I[i][0];
//		loc[i].y_ = I[i][1];
//	};
//	myfile.close();
//	return 0;
//}






void D_star::check_state(tuple<int, int>& t)
{
	if (this->h.find(t) == this->h.end())
	{
		this->h.insert(make_pair(t, 0));
	}
	if (this->tag.find(t) == this->tag.end())
	{
		this->tag.insert(make_pair(t, "New"));
	}
}



double D_star::get_kmin()
{
	/*拿到OPEN中最小值*/
	if (!this->OPEN.empty())
	{
		double min_value = 1000;
		for (auto it = OPEN.begin(); it != OPEN.end(); ++it)
		{
			if (it->second < min_value)
			{
				min_value = it->second;
			}
		}
		return min_value;
	}
	return -1;
}



tuple<tuple<int, int>, double>  D_star::min_state()
{
	/*弹出OPEN最小值*/
	if (!this->OPEN.empty())
	{
		double min_value = this->get_kmin();
		for (auto it = OPEN.begin(); it != OPEN.end(); ++it)
		{
			if (it->second == min_value)
			{
				tuple<tuple<int, int>, double> t = make_tuple(it->first, min_value); // 找了一天bug，就是这句double写成int了
				OPEN.erase(it);
				return t;
			}
		}
	}
	tuple<tuple<int, int>, int> t = make_tuple(tuple<int, int>(-1, -1), -1);
	return t;
}



void D_star::insert(tuple<int, int>& x, double& h_new)
{
	/*插入OPEN表并更新h*/
	double kx;
	if (tag[x] == "New")
	{
		kx = h_new;
	}
	if (tag[x] == "Open")
	{
		kx = OPEN[x] < h_new ? OPEN[x] : h_new;
	}
	if (tag[x] == "Closed")
	{
		kx = h[x] < h_new ? h[x] : h_new;
	}
	if (x == tuple<int, int>(7, 7))
	{
		int aaa = 0;
	}
	OPEN[x] = kx;
	h[x] = h_new;
	tag[x] = "Open";
}



double D_star::cost(tuple<int, int>& a, tuple<int, int>& b)
{
	/*欧式距离函数，当碰撞时返回一个非常大的值*/
	int x1 = get<0>(a);
	int y1 = get<1>(a);


	int x2 = get<0>(b);
	int y2 = get<1>(b);

	int obs_x = get<0>(obs_pos);
	int obs_y = get<1>(obs_pos);

	if ((sqrt((x1 - obs_x) * (x1 - obs_x) + (y1 - obs_y) * (y1 - obs_y)) < obs_r) ||
		(sqrt((x2 - obs_x) * (x2 - obs_x) + (y2 - obs_y) * (y2 - obs_y)) < obs_r))
	{
		return 1000;
	}
	else
	{
		return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) );
	}
}



vector<tuple<int, int>> D_star::children(tuple<int, int>& x)
{
	/*获取子节点坐标*/
	vector<tuple<int, int>> allchild;
	for (auto it = Alldirec.begin(); it != Alldirec.end(); ++it)
	{
		auto direc = it->first;
		int xx = get<0>(x) + get<0>(direc);
		int yy = get<1>(x) + get<1>(direc);
		tuple<int, int> child = make_tuple(xx, yy);
		int obs_x = get<0>(obs_pos);
		int obs_y = get<1>(obs_pos);
		if (sqrt((xx - obs_x) * (xx - obs_x) + (yy - obs_y) * (yy - obs_y)) <= obs_r) continue;
		if (xx < 0 || xx > 20 ||
			yy < 0 || yy > 20 )continue;
		allchild.push_back(child);
	}
	return allchild;
}




double D_star::process_state()
{
	/*核心函数*/
	tuple<tuple<int, int>, double> temp = this->min_state();
	tuple<int, int> x = get<0>(temp);
	double kold = get<1>(temp);
	this->tag[x] = "Closed";
	if (x == tuple<int, int>(-1, -1)) return -1;
	this->check_state(x);

	if (kold < h[x])
	{
		auto allchild = children(x);
		for (auto it = allchild.begin(); it != allchild.end(); ++it)
		{
			tuple<int, int> y = *it;
			check_state(y);
			double a = h[y] + cost(y, x);
			if (h[y] <= kold && h[x] > a)
			{
				b[x] = y;
				h[x] = a;
			}
		}
	}
	if (kold == h[x])
	{
		auto allchild = children(x);
		for (auto it = allchild.begin(); it != allchild.end(); ++it)
		{
			tuple<int, int> y = *it;
			check_state(y);
			double bb = h[x] + cost(x, y);
			if (tag[y] == "New" || (b[y] == x && h[y] != bb) || (b[y] != x && h[y] > bb))
			{
				b[y] = x;
				insert(y, bb);
			}
		}
	}
	else
	{
		auto allchild = children(x);
		for (auto it = allchild.begin(); it != allchild.end(); ++it)
		{
			tuple<int, int> y = *it;
			check_state(y);
			double bb = h[x] + cost(x, y);
			if (tag[y] == "New" || (b[y] == x && h[y] != bb))
			{
				b[y] = x;
				insert(y, bb);
			}
			else
			{
				if (b[y] != x && h[y] > bb)
				{
					insert(x, h[x]);
				}
				else
				{
					if (b[y] != x && h[y] > bb && tag[y] == "Closed" && h[y] == kold)
					{
						insert(y, h[y]);
					}
				}
			}
		}
	}
	return get_kmin();
}




void D_star::modify_cost(tuple<int, int>& x)
{
	auto xparent = b[x];
	if (tag[x] == "Closed")
	{
		double temp = h[xparent] + cost(x, xparent);
		insert(x, temp);
	}
}



void D_star::modify(tuple<int, int>& x)
{
	modify_cost(x);
	while (true)
	{
		double kmin = process_state();
		if (kmin >= h[x]) break;
	}
}



void D_star::get_path()
{
	/*获取路径*/
	path.clear();
	tuple<int, int> s = goal;
	tuple<int, int> x = start;
	path.push_back(x);
	do
	{
		x = b[x];
		path.push_back(x);
	} while (x != s);
}



void D_star::run()
{
	// D* 算法首先需要计算静态环境下的h
	OPEN[goal] = 0;
	tag[start] = "New";
	while (true)
	{
		cout << "程序正向执行了" << ++count << "个迭代回合。" << endl;
		process_state();
		if (tag[start] == "Closed") break;
	}
	get_path();
	save_path("path1.csv");
	cout << "记录已经保存" << endl;

	// D*面对变化后的环境进行小范围replan
	for (int i = 1; i < 3; ++i)
	{
		obs_pos = make_tuple(9 - i, 9 - i); // 新的障碍位置（这里可以设置为动态变化的位置，一个道理）
		auto s = start;
		while (s != goal)
		{
			auto sparent = b[s];
			if (cost(s, sparent) > 500)
			{
				modify(s);
				continue;
			}
			s = sparent;
		}
		get_path();
		char load_dir[10];
		sprintf_s(load_dir, "%s%d%s", "path", i + 1, ".csv");
		save_path(load_dir);
		cout << "记录已经保存" << endl;
	}
}



void D_star::save_path(string load_dir)
{
	/*保存数据*/
	ofstream ofs;
	ofs.open(load_dir, ios::out);
	for (auto it = path.begin(); it != path.end(); ++it)
	{
		tuple<int, int>xy = *it;
		int x = get<0>(xy);
		int y = get<1>(xy);
		if (it != path.end() - 1)
		{
			ofs << x << "," << y << endl;
		}
		else
		{
			ofs << x << "," << y;
		}
	}
	ofs.close();
}

