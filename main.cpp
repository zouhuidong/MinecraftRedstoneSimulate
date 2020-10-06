///////////////////////////////////////////////
//
//	MCRedstoneVirtualBox
//	main.cpp
//	�ҵ������ʯģ����
//	by huidong 2020.10.5 <mailkey@yeah.net>
//


#include <easyx.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <thread>

// �汾��Ϣ
char m_str_version[] = { "Version 1.0 ��ʽ��" };

//
//    ___________________________________________________________________
//   / Warning													      |X|\
//   |___________________________________________________________________|
//   |                                                                   |
//   |       /\                                                          |
//   |      / !\    Warning                                              |
//   |     /____\                                                        |
//   |                �����Ŀ�и��£������ main.cpp ������ע���ϵ�ʱ�䣬   |
//   |                m_str_version �ϵİ汾��HelpMenu�����ϵĸ�����־��    |
//   |___________________________________________________________________|
//
//

// ȫ�ֱ���������ͼ�������±�0��ʾδ��������������±�Ϊ1��ʾ���������

IMAGE m_img_rod[2];		// ����
IMAGE m_img_button[2];	// ��ť
IMAGE m_img_torche[2];	// ��ʯ���
IMAGE m_img_light[2];	// ��ʯ��
IMAGE m_img_relay[2];	// ��ʯ�м���
IMAGE m_img_cross;		// �������߰�

//
// �������߰���������MC���飬ֻ����Ϊƽ���·�������Ҫ�����
// ���������ڳ��������ĸ��ֻ����������ʱ��Ҫ�ٴ��ж�ͬ���������һ�������Ƿ��е���ܾ����˷����Ƿ��е�
// �ٸ����ӣ�
//     a
//     |
//  c����n����d
//     |
//     b
//
// n�ǽ������߰壬���a�е�ֻ�ܾ���b�е磬����Ӱ��c,d���Դ����ơ�
//

// ��ʯ��������
enum RedstoneObjectTypes
{
	RS_NULL,	// �������飨�޷��飩
	RS_POWDER,	// ��ʯ��
	RS_ROD,		// ����
	RS_BUTTON,	// ��ť
	RS_TORCHE,	// ��ʯ���
	RS_LIGHT,	// ��ʯ��
	RS_RELAY,	// �м���
	RS_CROSS	// �������߰�
};

// ��ʯ���鳯��
enum RedstoneTowards
{
	RS_TO_UP,	// ������
	RS_TO_DOWN,	// ������
	RS_TO_LEFT,	// ������
	RS_TO_RIGHT	// ������
};

// ��ʯ���鶨��
typedef struct RedstoneObject
{
	int nObj;		// ��������
	bool bPower;	// ����ֵ
	int nTowards;	// ���򣨽�����м�����

}RsObj;

// ��ʯ��ͼ
typedef struct RedstoneMap
{
	// ��ͼ���
	int w;
	int h;

	// ��ͼ
	RsObj** map;
}RsMap;


// �õ��շ���
RsObj GetNullObj()
{
	return RsObj{ RS_NULL ,false ,RS_TO_UP };
}

// ��ʼ����ʯ��ͼ
RsMap InitRsMap(int w, int h)
{
	RsMap map;
	map.w = w;
	map.h = h;

	map.map = new RsObj * [h];

	for (int i = 0; i < h; i++)
	{
		map.map[i] = new RsObj[w];

		for (int j = 0; j < w; j++)
		{
			map.map[i][j] = GetNullObj();
		}
	}

	return map;
}

// ���ط���ͼ��
void loadimages()
{
	loadimage(&m_img_rod[0], L"./res/objs/null/rod/rod.bmp");
	loadimage(&m_img_rod[1], L"./res/objs/power/rod/rod.bmp");

	loadimage(&m_img_button[0], L"./res/objs/null/button/button.bmp");
	loadimage(&m_img_button[1], L"./res/objs/power/button/button.bmp");

	loadimage(&m_img_torche[0], L"./res/objs/null/torche/torche.bmp");
	loadimage(&m_img_torche[1], L"./res/objs/power/torche/torche.bmp");

	loadimage(&m_img_light[0], L"./res/objs/null/light/light.bmp");
	loadimage(&m_img_light[1], L"./res/objs/power/light/light.bmp");

	loadimage(&m_img_relay[0], L"./res/objs/null/relay/relay.bmp");
	loadimage(&m_img_relay[1], L"./res/objs/power/relay/relay.bmp");

	loadimage(&m_img_cross, L"./res/objs/null/cross/cross.bmp");
}

// �ж�һ�����Ƿ�Ϊ�ź�Դ����
bool isPowerObj(RsObj obj)
{
	return obj.nObj == RS_ROD || obj.nObj == RS_BUTTON || obj.nObj == RS_TORCHE;
}

// �Ƿ�Ϊ��ͨ���飨�����Ƿ�ɵ��磩������ʯ��ѣ�
bool isNormalObj(RsObj obj)
{
	return (obj.nObj != RS_NULL && !isPowerObj(obj)) || obj.nObj == RS_TORCHE;
}

// �Ƿ�Ϊ�ɵ��緽�飨�����ź�Դ����ͨ���飩
bool isConductiveObj(RsObj obj)
{
	return obj.nObj != RS_NULL && obj.nObj != RS_LIGHT;
}

// �ж�һ�����������Ƿ��е���
bool isLeftPower(RsMap* map, int x, int y)
{
	if (x - 1 >= 0 && map->map[y][x - 1].bPower && isConductiveObj(map->map[y][x - 1]))
	{
		switch (map->map[y][x - 1].nObj)
		{
		case RS_RELAY:
			if (map->map[y][x - 1].nTowards == RS_TO_RIGHT)
			{
				return true;
			}
			break;

		case RS_CROSS:
			if (isLeftPower(map, x - 1, y))
			{
				return true;
			}
			break;

		default:
			return true;
			break;
		}
	}

	return false;
}

// �ж�һ��������ұ��Ƿ��е���
bool isRightPower(RsMap* map, int x, int y)
{
	if (x + 1 < map->w && map->map[y][x + 1].bPower && isConductiveObj(map->map[y][x + 1]))
	{
		switch (map->map[y][x + 1].nObj)
		{
		case RS_RELAY:
			if (map->map[y][x + 1].nTowards == RS_TO_LEFT)
			{
				return true;
			}
			break;

		case RS_CROSS:
			if (isRightPower(map, x + 1, y))
			{
				return true;
			}
			break;

		default:
			return true;
			break;
		}
	}

	return false;
}

// �ж�һ��������ϱ��Ƿ��е���
bool isUpPower(RsMap* map, int x, int y)
{
	if (y - 1 >= 0 && map->map[y - 1][x].bPower && isConductiveObj(map->map[y - 1][x]))
	{
		switch (map->map[y - 1][x].nObj)
		{
		case RS_RELAY:
			if (map->map[y - 1][x].nTowards == RS_TO_DOWN)
			{
				return true;
			}
			break;

		case RS_CROSS:
			if (isUpPower(map, x, y - 1))
			{
				return true;
			}
			break;

		default:
			return true;
			break;
		}
	}

	return false;
}

// �ж�һ��������±��Ƿ��е���
bool isDownPower(RsMap* map, int x, int y)
{
	if (y + 1 < map->h && map->map[y + 1][x].bPower && isConductiveObj(map->map[y + 1][x]))
	{
		switch (map->map[y + 1][x].nObj)
		{
		case RS_RELAY:
			if (map->map[y + 1][x].nTowards == RS_TO_UP)
			{
				return true;
			}
			break;

		case RS_CROSS:
			if (isDownPower(map, x, y + 1))
			{
				return true;
			}
			break;

		default:
			return true;
			break;
		}
	}

	return false;
}

// ������Χ���������ж����Ƿ�Ӧ���е�
bool isMePower(RsMap* map, int x, int y)
{
	switch (map->map[y][x].nObj)
	{
	case RS_RELAY:

		switch (map->map[y][x].nTowards)
		{
		case RS_TO_RIGHT:
			if (isLeftPower(map, x, y))
			{
				return true;
			}
			break;

		case RS_TO_LEFT:
			if (isRightPower(map, x, y))
			{
				return true;
			}
			break;

		case RS_TO_UP:
			if (isDownPower(map, x, y))
			{
				return true;
			}
			break;

		case RS_TO_DOWN:
			if (isUpPower(map, x, y))
			{
				return true;
			}
			break;
		}

		break;

	default:
		if (
			isLeftPower(map, x, y) ||
			isRightPower(map, x, y) ||
			isUpPower(map, x, y) ||
			isDownPower(map, x, y)
			)
		{
			return true;
		}
		break;
	}

	return false;
}

// ִ�й�����ͨ����Ĵ�������ʯ��ѣ�
void RunObj(RsMap* map, bool**& pPass, int x, int y)
{
	// �Ѿ����ʹ��������ظ����ʣ���ֹ��ɵݹ���ѭ��
	if (pPass[y][x])
		return;

	RsObj* me = &map->map[y][x];

	// ���������������
	if (isMePower(map, x, y))
	{
		// ����Ǻ�ʯ��ѱ������źţ���ôֱ��Ϩ��
		if (me->nObj == RS_TORCHE)
		{
			me->bPower = false;
			return;
		}

		// ��������Ļ��ͱ�����
		else
		{
			me->bPower = true;
		}
	}

	// ���û������������
	else
	{
		// ����Ǻ�ʯ���û�����źţ���ô����������
		if (me->nObj == RS_TORCHE)
		{
			me->bPower = true;
		}

		// ��������Ͱ���
		else
		{
			me->bPower = false;
		}

		return;
	}

	pPass[y][x] = true;

	// �ж������Ƿ���Լ��������źţ������´��䡣
	if (x - 1 >= 0 && isNormalObj(map->map[y][x - 1]))
	{
		RunObj(map, pPass, x - 1, y);
	}

	if (x + 1 < map->w && isNormalObj(map->map[y][x + 1]))
	{
		RunObj(map, pPass, x + 1, y);
	}

	if (y - 1 >= 0 && isNormalObj(map->map[y - 1][x]))
	{
		RunObj(map, pPass, x, y - 1);
	}

	if (y + 1 < map->h && isNormalObj(map->map[y + 1][x]))
	{
		RunObj(map, pPass, x, y + 1);
	}
}

// ִ�й�������Դ�Ĵ���
void RunPower(RsMap* map, bool**& pPass, int x, int y)
{
	// ÿ�δ����ź�Դǰ�������ʼ�¼���
	for (int i = 0; i < map->h; i++)
		for (int j = 0; j < map->w; j++)
			pPass[i][j] = false;

	pPass[y][x] = true;

	// �ź�Դ�У�ֻ�к�ʯ��ѻ�����ⲿ�ź����룬������ⲿ�ź����룬����Ϩ��
	if (map->map[y][x].nObj == RS_TORCHE)
	{
		if (isMePower(map, x, y))
		{
			map->map[y][x].bPower = false;
			return;
		}
	}

	if (x - 1 >= 0 && isNormalObj(map->map[y][x - 1]))
	{
		RunObj(map, pPass, x - 1, y);
	}

	if (x + 1 < map->w && isNormalObj(map->map[y][x + 1]))
	{
		RunObj(map, pPass, x + 1, y);
	}

	if (y - 1 >= 0 && isNormalObj(map->map[y - 1][x]))
	{
		RunObj(map, pPass, x, y - 1);
	}

	if (y + 1 < map->h && isNormalObj(map->map[y + 1][x]))
	{
		RunObj(map, pPass, x, y + 1);
	}
}

// ���к�ʯ��ͼ
// repeat ��ʾ��ͼ����ѭ���������ɲ��ѭ���ɱ���һЩbug�ĳ��֣������3�Ρ�
void RunRsMap(RsMap* map, int repeat = 3)
{
	// �洢��ͼ��ÿ����ķ��ʼ�¼
	bool** pPass = new bool* [map->h];
	for (int i = 0; i < map->h; i++)
	{
		pPass[i] = new bool[map->w];
		for (int j = 0; j < map->w; j++)
			pPass[i][j] = false;
	}

	for (int n = 0; n < repeat; n++)
	{
		// ����ʹ���в��ǵ�Դ�ķ��鶼û��
		for (int i = 0; i < map->w; i++)
		{
			for (int j = 0; j < map->h; j++)
			{
				if (!isPowerObj(map->map[j][i]))
				{
					map->map[j][i].bPower = false;
				}
			}
		}

		for (int i = 0; i < map->w; i++)
		{
			for (int j = 0; j < map->h; j++)
			{
				if (isPowerObj(map->map[j][i]))
				{
					RunPower(map, pPass, i, j);
				}
			}
		}

		// ������ͼ������������
		for (int i = 0; i < map->w; i++)
		{
			for (int j = 0; j < map->h; j++)
			{
				// �������Χ������
				if (!isMePower(map, i, j) && isNormalObj(map->map[j][i]))
				{
					// �и���������Ǻ�ʯ��ѣ������û�����ܣ��Ͱ������źŻָ���true
					if (map->map[j][i].nObj == RS_TORCHE)
					{
						map->map[j][i].bPower = true;
					}

					// ��ͨ�����������źŵ�
					else
					{
						map->map[j][i].bPower = false;
					}
				}
			}
		}
	}

	// �ͷ������ڴ�
	for (int i = 0; i < map->h; i++)
		delete[] pPass[i];
	delete[] pPass;
	pPass = NULL;
}

// ������Ʒ����ͼ
void PutObjectToRsMap(RsMap* map, int x, int y, int object_id, int direction = RS_TO_UP)
{
	RsObj obj = GetNullObj();
	obj.nObj = object_id;

	switch (object_id)
	{
	case RS_TORCHE:
		obj.bPower = true;
		break;
	case RS_RELAY:
		obj.nTowards = direction;
		break;
	}

	map->map[y][x] = obj;
}

// ѡ���ļ�
// isSave��־���Ƿ�Ϊ����ģʽ
const WCHAR* SelectFile(bool isSave = false)
{
	OPENFILENAME ofn;
	static WCHAR szFile[256];
	static WCHAR szFileTitle[256];
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetConsoleWindow();
	ofn.lpstrFilter = L"Redstone map File(*.rsp)\0*.rsp\0All File(*.*)\0*.*\0";
	ofn.lpstrDefExt = L"rsp";	// ָ�����Ĭ����չ���Ļ��塣����û�����������չ����GetOpenFileName��GetSaveFileName���������չ�����ļ����С�
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFileTitle = szFileTitle;
	ofn.nMaxFileTitle = sizeof(szFileTitle);
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

	if (isSave)
	{
		if (GetSaveFileName(&ofn))
		{
			return szFile;
		}
	}
	else
	{
		if (GetOpenFileName(&ofn))
		{
			return szFile;
		}
	}

	return _T("");
}

// �ж��ַ��Ƿ�Ϊ����
bool isNum(char ch)
{
	return ch >= '0' && ch <= '9';
}

// �ж�һ�ַ����Ƿ���ȫΪ����
bool isAllNum(const char* str)
{
	for (int i = 0; i < (int)strlen(str); i++)
		if (!isNum(str[i]))
			return false;
	return true;
}

// �ڵ�ǰ�������¶�ȡһ������
bool ReadNum(const char* str, int& index, int& num)
{
	char* chNum = new char[strlen(str) + 1];
	memset(chNum, 0, strlen(str) + 1);
	int num_index = 0;

	for (; index < (int)strlen(str); index++, num_index++)
	{
		if (isNum(str[index]))
		{
			chNum[num_index] = str[index];
		}
		else
		{
			break;
		}
	}

	if (num_index > 0)
	{
		num = atoi(chNum);
		return true;
	}

	return false;
}

// ɾ����ͼ
void DeleteRsMap(RsMap* map)
{
	if (map->map != NULL)
	{
		for (int i = 0; i < map->h; i++)
		{
			delete[] map->map[i];
		}

		delete[] map->map;
		map->map = NULL;
	}
}

// �����ͼ��С
void ReSizeRsMap(RsMap* map, int w, int h)
{
	RsMap newmap = InitRsMap(w, h);

	int old_w = map->w;
	int old_h = map->h;

	if (old_w > w)
	{
		old_w = w;
	}
	if (old_h > h)
	{
		old_h = h;
	}

	for (int i = 0; i < old_w; i++)
	{
		for (int j = 0; j < old_h; j++)
		{
			newmap.map[j][i] = map->map[j][i];
		}
	}

	DeleteRsMap(map);
	*map = newmap;
}

// ������Ŀ
bool SaveProject(RsMap map, const WCHAR* strFileName)
{
	if (!lstrlen(strFileName))
	{
		return false;
	}

	FILE* fp;
	int nSize = WideCharToMultiByte(CP_ACP, 0, strFileName, -1, NULL, 0, NULL, NULL);
	char* chFileName = new char[nSize];
	memset(chFileName, 0, nSize);
	WideCharToMultiByte(CP_ACP, 0, strFileName, -1, chFileName, nSize, NULL, NULL);

	if (fopen_s(&fp, chFileName, "wt+") != 0)
	{
		return false;
	}

	char chW[12] = { 0 };
	char chH[12] = { 0 };
	_itoa_s(map.w, chW, 10);
	_itoa_s(map.h, chH, 10);

	fputs(chW, fp);
	fputs(" ", fp);
	fputs(chH, fp);
	fputs("\n", fp);

	for (int i = 0; i < map.h; i++)
	{
		for (int j = 0; j < map.w; j++)
		{
			char chObj[6] = { 0 };
			char chTowards[6] = { 0 };
			char chPower = '0';
			_itoa_s(map.map[i][j].nObj, chObj, 10);
			_itoa_s(map.map[i][j].nTowards, chTowards, 10);
			if (map.map[i][j].bPower && map.map[i][j].nObj != RS_BUTTON)
				chPower = '1';

			fputs(chObj, fp);
			fputc(',', fp);
			fputs(chTowards, fp);
			fputc(',', fp);
			fputc(chPower, fp);

			if (j + 1 != map.w)
			{
				fputc(' ', fp);
			}
		}

		fputc('\n', fp);
	}

	fclose(fp);
	return true;
}

// ����Ŀ
RsMap OpenProject(const WCHAR* strFileName)
{
	if (!lstrlen(strFileName))
	{
		return RsMap{};
	}

	FILE* fp;
	int nSize = WideCharToMultiByte(CP_ACP, 0, strFileName, -1, NULL, 0, NULL, NULL);
	char* chFileName = new char[nSize];
	memset(chFileName, 0, nSize);
	WideCharToMultiByte(CP_ACP, 0, strFileName, -1, chFileName, nSize, NULL, NULL);

	fopen_s(&fp, chFileName, "r+");
	char* chProject = new char[_filelength(_fileno(fp)) + 1];
	memset(chProject, 0, _filelength(_fileno(fp)) + 1);
	fread_s(chProject, _filelength(_fileno(fp)) + 1, _filelength(_fileno(fp)), 1, fp);

	fclose(fp);

	// �����ļ�

	// �ļ���ʽ��
	/*
	��ͼ�� ��ͼ��
	����id,����,���� ����id,����,���� ......
	����id,����,���� ����id,����,���� ......
	......
	����id,����,���� ����id,����,���� ......
	*/

	// ��ȡ��ͼ���
	int nMap_w, nMap_h;
	int index = 0;

	if (!ReadNum(chProject, index, nMap_w))
		return RsMap{};
	index++;
	if (!ReadNum(chProject, index, nMap_h))
		return RsMap{};
	index++;

	RsMap map = InitRsMap(nMap_w, nMap_h);

	for (int i = 0; i < nMap_h; i++)
	{
		for (int j = 0; j < nMap_w; j++)
		{
			int nObj = RS_NULL;
			int nTowards = RS_TO_UP;
			int nPower = false;

			// ����Ʒid
			if (!ReadNum(chProject, index, nObj))
				return RsMap{};
			index++;

			// ������
			if (!ReadNum(chProject, index, nTowards))
				return RsMap{};
			index++;

			// ������
			if (!ReadNum(chProject, index, nPower))
				return RsMap{};
			index++;

			RsObj rsobj = GetNullObj();
			rsobj.nObj = nObj;
			rsobj.nTowards = nTowards;
			rsobj.bPower = (bool)nPower;

			map.map[i][j] = rsobj;
		}
	}

	return map;
}

// ������Ŀ
// out ����Ŀ
// in ��������Ŀ
// x,y ��������Ŀλ������Ŀ�����Ͻ�����
void ImportProject(RsMap* out, RsMap in, int x, int y)
{
	for (int jo = y, ji = 0; jo < out->h && ji < in.h; jo++, ji++)
	{
		for (int io = x, ii = 0; io < out->w && ii < in.w; io++, ii++)
		{
			out->map[jo][io] = in.map[ji][ii];
		}
	}
}

// ��ʼ����
RsMap StartMenu()
{
	printf("MCRedstoneVirtualBox ���ߣ�huidong <mailkey@yeah.net>\n");
	printf("%s\n", m_str_version);
	printf("����Ŀ���� O�� �� �½���Ŀ���� N����");

	while (true)
	{
		if (GetKeyState('O') & 0x80)
		{
			printf("\n���ص�ͼ�С������Ժ�\n");
			return OpenProject(SelectFile());
		}

		else if (GetKeyState('N') & 0x80)
		{
			int w = 0, h = 0;
		again:
			printf("\n��ͼ��С��������س�����������ٻس�����\n");
			scanf_s("%d", &w);
			scanf_s("%d", &h);

			if (w <= 0 || h <= 0)
			{
				printf("��ͼ��С����С�ڵ���0�������䡣");
				goto again;
			}

			return InitRsMap(w, h);
		}

		Sleep(10);

		// ��ռ��̻�����
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	}

	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}

// �õ���ͼ����
IMAGE* GetRsMapImage(RsMap* map, bool bShowXY)
{
	const double PI = 3.141592653589793846;
	const double DEGREE = PI / 180;

	int nObjSize = m_img_light[0].getwidth();

	IMAGE* imgMap = new IMAGE;
	imgMap->Resize(map->w * nObjSize, map->h * nObjSize);

	IMAGE* pOld = GetWorkingImage();

	SetWorkingImage(imgMap);
	settextcolor(WHITE);
	settextstyle(10, 0, L"����");
	setbkmode(TRANSPARENT);

	for (int x = 0; x < map->w; x++)
	{
		for (int y = 0; y < map->h; y++)
		{
			IMAGE powder(nObjSize, nObjSize);	// ʵʱ���ƺ�ʯ�۵Ļ���
			IMAGE relay(nObjSize, nObjSize);	// ʵʱ���ƺ�ʯ�м����Ļ���
			RsObj me = map->map[y][x];
			RsObj up, down, left, right;

			if (y - 1 >= 0)
				up = map->map[y - 1][x];
			if (y + 1 < map->h)
				down = map->map[y + 1][x];
			if (x - 1 >= 0)
				left = map->map[y][x - 1];
			if (x + 1 < map->w)
				right = map->map[y][x + 1];

			switch (me.nObj)
			{
			case RS_NULL:
				break;

			case RS_POWDER:

				SetWorkingImage(&powder);

				if (me.bPower)
				{
					setfillcolor(RGB(255, 0, 0));
					setlinecolor(RGB(255, 0, 0));
				}

				else
				{
					setfillcolor(RGB(100, 0, 0));
					setlinecolor(RGB(100, 0, 0));
				}

				fillcircle(nObjSize / 2, nObjSize / 2, 5);
				setlinestyle(PS_SOLID, 12);

				// ʵʱ���ƺ�ʯ��
				if (y - 1 >= 0 && up.nObj != RS_NULL)
					line(nObjSize / 2, nObjSize / 2, nObjSize / 2, 0);	// line to up

				if (y + 1 < map->h && down.nObj != RS_NULL)
					line(nObjSize / 2, nObjSize / 2, nObjSize / 2, nObjSize);	// line to down

				if (x - 1 >= 0 && left.nObj != RS_NULL)
					line(nObjSize / 2, nObjSize / 2, 0, nObjSize / 2);	// line to left

				if (x + 1 < map->w && right.nObj != RS_NULL)
					line(nObjSize / 2, nObjSize / 2, nObjSize, nObjSize / 2);	// line to right

				SetWorkingImage(imgMap);

				putimage(x * nObjSize, y * nObjSize, &powder);

				break;

			case RS_ROD:
				putimage(x * nObjSize, y * nObjSize, &m_img_rod[me.bPower]);
				break;

			case RS_BUTTON:
				putimage(x * nObjSize, y * nObjSize, &m_img_button[me.bPower]);
				break;

			case RS_TORCHE:
				putimage(x * nObjSize, y * nObjSize, &m_img_torche[me.bPower]);
				break;

			case RS_LIGHT:
				putimage(x * nObjSize, y * nObjSize, &m_img_light[me.bPower]);
				break;

			case RS_RELAY:

				// ��ת�м���
				switch (me.nTowards)
				{
				case RS_TO_UP:
					relay = m_img_relay[me.bPower];
					break;
				case RS_TO_LEFT:
					rotateimage(&relay, &m_img_relay[me.bPower], DEGREE * 90);
					break;
				case RS_TO_DOWN:
					rotateimage(&relay, &m_img_relay[me.bPower], DEGREE * 180);
					break;
				case RS_TO_RIGHT:
					rotateimage(&relay, &m_img_relay[me.bPower], DEGREE * 270);
					break;
				}

				putimage(x * nObjSize, y * nObjSize, &relay);

				break;

			case RS_CROSS:
				putimage(x * nObjSize, y * nObjSize, &m_img_cross);
				break;
			}

			// �������
			if (bShowXY)
			{
				TCHAR strX[12] = { 0 };
				TCHAR strY[12] = { 0 };
				wsprintf(strX, L"x:%d", x);
				wsprintf(strY, L"y:%d", y);
				outtextxy(x * nObjSize, y * nObjSize, strX);
				outtextxy(x * nObjSize, y * nObjSize + textheight('0'), strY);
			}
		}
	}

	SetWorkingImage(pOld);

	return imgMap;
}

// �������з������
// cmd ԭ����
// chCmdsArray_out ��������б�
// nArgsNum_out �����������
void GetArguments(const char* cmd, char*** chCmdsArray_out, int* nArgsNum_out)
{
	// ��������
	int nArgsNum = 1;
	for (int i = 0; i < (int)strlen(cmd); i++)
		if (cmd[i] == ' ')
			nArgsNum++;

	int nMaxCmdSize = strlen(cmd) + 1;
	char** chCmdsArray = new char* [nArgsNum];
	for (int i = 0; i < nArgsNum; i++)
	{
		chCmdsArray[i] = new char[nMaxCmdSize];
		memset(chCmdsArray[i], 0, nMaxCmdSize);
	}

	// ����ÿ������
	for (int nIndex = 0, nMainIndex = 0, nArgNum = 0; nIndex < (int)strlen(cmd); nIndex++, nMainIndex++)
	{
		if (cmd[nMainIndex] == ' ')
		{
			nIndex = -1;
			nArgNum++;
			continue;
		}

		chCmdsArray[nArgNum][nIndex] = cmd[nMainIndex];
	}

	*chCmdsArray_out = chCmdsArray;
	*nArgsNum_out = nArgsNum;
}

// �����˵�
void HelpMenu()
{
	int page = 0;
	int pages_num = 5;
	while (true)
	{
		system("cls");

		switch (page)
		{
		case 0:
			printf(
				"********************ָ���********************\n"
				"* xy\t\t\t����/�ر���ʾ����\n"
				"* cls\t\t\t����\n"
				"* ����x ����y\t\t��ĳλ�÷��ú�ʯ�ۣ�����Ǹ�λ��������Ʒ��������Ǹ���Ʒ\n"
				"* ����x ����y ��Ʒ (����)\t��ĳλ�÷���ĳ��Ʒ������������Ʒ���и��ǡ����ַ�������ó���������Ĭ�����ϡ�\n"
				"* ����x ����y ����\t����ĳλ�õ���Ʒ�ĳ�����Ժ�ʯ�м�����\n"
				"* . ����x ����y\t\t���ĳλ�õĿ��أ������ˣ���ť��\n"
				"* resize �� ��\t\t�����ͼ��С\n"
				"* save\t\t\t�����ͼΪ��Ŀ�ļ�\n"
				"* reset_map_offset\t\t����ͼ�����xyƫ�ƶ��ص�0\n"
				"* up ƫ����\t\t����ͼ����ƫ��n����λ��һ��λ��10����\n"
				"* down ƫ����\t\t����ͼ����ƫ��n����λ��һ��λ��10����\n"
				"* left ƫ����\t\t����ͼ����ƫ��n����λ��һ��λ��10����\n"
				"* right ƫ����\t\t����ͼ����ƫ��n����λ��һ��λ��10����\n"
				"* zoom ���Ŵ�С\t\t����ͼ���������ٰٷֱ�\n"
				"* zoom+ ���Ŵ�С\t\t�ڵ�ͼԭ�ȵ����Ż����ϣ��ٷŴ���ٸ��ٷֱ�\n"
				"* zoom- ���Ŵ�С\t\t�ڵ�ͼԭ�ȵ����Ż����ϣ��ټ�С���ٸ��ٷֱ�\n"
				"* get_map_offset\t\t�õ���ͼ�����xyƫ����\n"
				"* get_map_zoom\t\t�õ���ͼ��������Ŵ�С\n"
				"* import ����x ����y\t\t����һ����Ŀ����ǰ��Ŀ��ָ������Ŀ�����Ͻ�λ��\n"
				"* line ����x1 ����y1 ����x2 ����y2 (��Ʒ)\t����ʯֱ�ߣ������������������ĩβ������Ʒ����ʾ��ĳ�������ֱ�ߡ�\n"
				"* clear ����x1 ����y1 ����x2 ����y2\t�Կ������鸲��(x1,y1)��(x2,y2)�����з��顣\n"
				"* \n"
			);
			break;
		case 1:
			printf(
				"********************��Ʒ���Ʊ�********************\n"
				"* null\t\t\t����Ʒ\n"
				"* powder\t\t\t��ʯ��\n"
				"* rod\t\t\t����\n"
				"* button\t\t\t��ť\n"
				"* torche\t\t\t��ʯ���\n"
				"* light\t\t\t��ʯ��\n"
				"* relay\t\t\t��ʯ�м��������з�ֹ�źŵ����Ĺ��ܣ�\n"
				"* \n"
			);
			break;
		case 2:
			printf(
				"********************�����********************\n"
				"* up\t\t\t����\n"
				"* down\t\t\t����\n"
				"* left\t\t\t����\n"
				"* right\t\t\t����\n"
				"* \n"
			);
			break;
		case 3:
			printf(
				"********************����********************\n"
				"* ԭ���ߣ�huidong <mailkey@yeah.net> �� 2020.10.4 �з��ڲ�� Ver0.1��2020.10.5 �з���ʽ�� Ver1.0\n"
				"* ��ӭ�����ҵĸ�����վ< http://www.huidong.xyz/ >\n"
				"* �ְ汾��%s\n"
				"* ��л��\n"
				"* ckj <emil09_chen@126.com> Ϊ��Ŀ��ơ��㷨�Ż��ṩ�˱���Ľ��顣\n"
				"* ���� ����Ŀ���ԵĹ�����֧��\n"
				"* \n"
				, m_str_version);
			break;
		case 4:
			printf(
				"********************������־********************\n"
				"* 2020.10.4	�ڲ�� Ver 0.1	����ʵ�ֳ���\n"
				"* 2020.10.4	�ڲ�� Ver 0.2\n"
				"*				���½������߰壬������Ŀ���빦��\n"
				"* 2020.10.5	�ڲ�� Ver 0.3\n"
				"*				����ͼ�ν��洰�ڴ�С�������д��ڴ�С�����������У�ʹ���������㡣\n"
				"*				�������ƺ�ʯֱ�ߣ�������������Ż��㷨��\n"
				"* 2020.10.5	��ʽ�� Ver 1.0\n"
				"*				����������󣬻���������ȫ��ʵ�֡�\n"
				"* \n"
			);
			break;
		}

		printf("\n��ǰҳ����%d / %d����a,d����ҳ�����ո���˳������˵���\n", page + 1, pages_num);

		char input = (char)_getch();

		switch (input)
		{
		case 'a':
			if (page - 1 >= 0)
				page--;
			break;
		case 'd':
			if (page + 1 < pages_num)
				page++;
			break;
		case ' ':
			system("cls");
			return;
			break;
		}
	}
}

// ���ַ���ת����ID��
// str ��ʾԭ�ַ���
// type_out ��ʾID���ͣ�Ϊ0��ʾ��Ʒ��Ϊ1��ʾ����
// id_out ��ʾID
// ����������ַ����Ƿ�Ϸ�
bool GetIdFromString(const char* str, int* type_out, int* id_out)
{
	const int OBJECT = 0;
	const int DIRECTION = 1;

	// object
	if (strcmp(str, "null") == 0)
	{
		*type_out = OBJECT;
		*id_out = RS_NULL;
	}
	else if (strcmp(str, "powder") == 0)
	{
		*type_out = OBJECT;
		*id_out = RS_POWDER;
	}
	else if (strcmp(str, "rod") == 0)
	{
		*type_out = OBJECT;
		*id_out = RS_ROD;
	}
	else if (strcmp(str, "button") == 0)
	{
		*type_out = OBJECT;
		*id_out = RS_BUTTON;
	}
	else if (strcmp(str, "torche") == 0)
	{
		*type_out = OBJECT;
		*id_out = RS_TORCHE;
	}
	else if (strcmp(str, "light") == 0)
	{
		*type_out = OBJECT;
		*id_out = RS_LIGHT;
	}
	else if (strcmp(str, "relay") == 0)
	{
		*type_out = OBJECT;
		*id_out = RS_RELAY;
	}

	else if (strcmp(str, "cross") == 0)
	{
		*type_out = OBJECT;
		*id_out = RS_CROSS;
	}

	// direction
	else if (strcmp(str, "up") == 0)
	{
		*type_out = DIRECTION;
		*id_out = RS_TO_UP;
	}
	else if (strcmp(str, "down") == 0)
	{
		*type_out = DIRECTION;
		*id_out = RS_TO_DOWN;
	}
	else if (strcmp(str, "left") == 0)
	{
		*type_out = DIRECTION;
		*id_out = RS_TO_LEFT;
	}
	else if (strcmp(str, "right") == 0)
	{
		*type_out = DIRECTION;
		*id_out = RS_TO_RIGHT;
	}

	// ���������
	else
	{
		return false;
	}

	return true;
}

// ��������Ϸ��ԣ��Ƿ񳬳���ͼ��
bool PointIsInMap(RsMap* map, int x, int y)
{
	return x >= 0 && x < map->w&& y >= 0 && y < map->h;
}

// ͼƬ����
// width, height ������ͼƬ��С
// img ԭͼ��
void ImageToSize(int width, int height, IMAGE* img)
{
	IMAGE* pOldImage = GetWorkingImage();
	SetWorkingImage(img);

	IMAGE temp_image(width, height);

	StretchBlt(
		GetImageHDC(&temp_image), 0, 0, width, height,
		GetImageHDC(img), 0, 0,
		getwidth(), getheight(),
		SRCCOPY
	);

	Resize(img, width, height);
	putimage(0, 0, &temp_image);

	SetWorkingImage(pOldImage);
}

// ����ͼ��
void ProcessingImage(RsMap* map, bool* bShowXY, int offset_x, int offset_y, double zoom)
{
	IMAGE* img = GetRsMapImage(map, *bShowXY);
	ImageToSize((int)(img->getwidth() * zoom), (int)(img->getheight() * zoom), img);
	SetWorkingImage();
	cleardevice();
	putimage(offset_x, offset_y, img);
	delete img;
}

// ���һ����ť��������
void ClickButton(RsMap* map, int x, int y, bool* bShowXY, int offset_x, int offset_y, double zoom)
{
	int delay = 2000;

	if (map->map[y][x].nObj == RS_BUTTON)
	{
		map->map[y][x].bPower = true;
		RunRsMap(map);

		ProcessingImage(map, bShowXY, offset_x, offset_y, zoom);	// �ֶ��ػ�

		Sleep(delay);
		map->map[y][x].bPower = false;
		RunRsMap(map);

		ProcessingImage(map, bShowXY, offset_x, offset_y, zoom);
	}
}

// �õ�������ҺϷ������꣨�����������ʹ�ã�
void GetSortingPoint(RsMap* map, int* x1, int* y1, int* x2, int* y2)
{
	// ȷ��x1 <= x2, y1 <= y2
	if (*x1 > * x2)
	{
		int temp = *x1;
		*x1 = *x2;
		*x2 = temp;
	}
	if (*y1 > *y2)
	{
		int temp = *y1;
		*y1 = *y2;
		*y2 = temp;
	}

	// ȷ������Ϸ�
	if (*x1 < 0)
		*x1 = 0;
	if (*x2 >= map->w)
		*x2 = map->w - 1;
	if (*y1 < 0)
		*y1 = 0;
	if (*y2 >= map->h)
		*y2 = map->h - 1;
}

// �ں�ʯ��ͼ�л��ƺ�ʯֱ�ߣ���������������ָ����������ʯֱ�ߵ���Ʒ
// �����Ƿ���Ƴɹ������Ʋ��ɹ���ԭ����ֱ��������
bool LineRsMap(RsMap* map, int x1, int y1, int x2, int y2, int object)
{
	// ��������
	GetSortingPoint(map, &x1, &y1, &x2, &y2);

	if (x1 == x2)
	{
		for (int i = y1; i <= y2; i++)
		{
			PutObjectToRsMap(map, x1, i, object);
		}
	}
	else if (y1 == y2)
	{
		for (int i = x1; i <= x2; i++)
		{
			PutObjectToRsMap(map, i, y1, object);
		}
	}
	else
	{
		return false;
	}

	return true;
}

// �����ʯ��ͼ�е�һ������
void ClearRsMap(RsMap* map, int x1, int y1, int x2, int y2)
{
	// ��������
	GetSortingPoint(map, &x1, &y1, &x2, &y2);

	for (int i = y1; i <= y2; i++)
	{
		for (int j = x1; j <= x2; j++)
		{
			PutObjectToRsMap(map, j, i, RS_NULL);
		}
	}
}

// �����û��������루������
void ProcessingCommand(RsMap* map, bool* p_bShowXY, int* offset_x, int* offset_y, double* zoom)
{
	// ��ͼ�ƶ��ĵ�λ��С�����أ�
	int offset_unit_size = 10;

	// ���������
	int nMaxInputdSize = 1024;
	char* chCmd = new char[nMaxInputdSize];
	memset(chCmd, 0, nMaxInputdSize);

	gets_s(chCmd, nMaxInputdSize);

	// ��������
	int nArgsNum;
	char** chCmdsArray;

	// ��������
	GetArguments(chCmd, &chCmdsArray, &nArgsNum);

	// �����Ƿ�����ʾ����
	if (nArgsNum == 1 && strcmp(chCmdsArray[0], "xy") == 0)
	{
		*p_bShowXY = !(*p_bShowXY);
	}

	// ����
	else if (nArgsNum == 1 && strcmp(chCmdsArray[0], "help") == 0)
	{
		HelpMenu();
		return;
	}

	// ����
	else if (nArgsNum == 1 && strcmp(chCmdsArray[0], "cls") == 0)
	{
		system("cls");
		return;
	}

	// ֱ�����������ʾ��ĳλ�÷��ú�ʯ�ۣ�����Ǹ�λ��������Ʒ��������Ǹ���Ʒ
	else if (nArgsNum == 2 && isAllNum(chCmdsArray[0]) && isAllNum(chCmdsArray[1]))
	{
		int x = atoi(chCmdsArray[0]);
		int y = atoi(chCmdsArray[1]);

		if (!PointIsInMap(map, x, y))
		{
			printf("����Խ�硣\n");
			return;
		}

		if (map->map[y][x].nObj != RS_NULL)
		{
			PutObjectToRsMap(map, x, y, RS_NULL);
		}
		else
		{
			PutObjectToRsMap(map, x, y, RS_POWDER);
		}
	}

	// ���� ���� ��Ʒ ������
	// ���������
	// ��ĳλ�÷���ĳ��Ʒ������Ǹ�λ��������Ʒ���򸲸��Ǹ���Ʒ����ʯ�м����г����趨������������ʾ����
	// ����ĳλ�õ���Ʒ�ĳ�����Ժ�ʯ�м�����
	else if ((nArgsNum == 3 || nArgsNum == 4) && isAllNum(chCmdsArray[0]) && isAllNum(chCmdsArray[1]))
	{
		int type;
		int id;
		if (!GetIdFromString(chCmdsArray[2], &type, &id))
		{
			printf("�������ƴ���\n");
			return;
		}

		int x = atoi(chCmdsArray[0]);
		int y = atoi(chCmdsArray[1]);

		if (!PointIsInMap(map, x, y))
		{
			printf("����Խ�硣\n");
			return;
		}

		if (type == 0)
		{
			PutObjectToRsMap(map, x, y, id);

			// �����ĸ�������˵������һ������
			if (nArgsNum == 4)
			{
				int type2;
				int direction;
				if (!GetIdFromString(chCmdsArray[3], &type2, &direction))
				{
					printf("����ִ�в���ȫ�����﷨�������ֹ�ˡ�����ԭ�򣺵��ĸ�����δ��ָ�����鳯��\n");
					return;
				}

				if (type2 == 1)
				{
					map->map[y][x].nTowards = direction;
				}
				else
				{
					printf("����ִ�в���ȫ�����﷨�������ֹ�ˡ�����ԭ�򣺵��ĸ�����δ��ָ�����鳯��\n");
					return;
				}
			}
		}
		else if (type == 1)
		{
			if (map->map[y][x].nObj == RS_NULL)
			{
				printf("�����겢����Ʒ���顣\n");
				return;
			}
			else
			{
				map->map[y][x].nTowards = id;
			}
		}
	}

	// . ����x ����y
	else if (nArgsNum == 3 && strcmp(chCmdsArray[0], ".") == 0 && isAllNum(chCmdsArray[1]) && isAllNum(chCmdsArray[2]))
	{
		int x = atoi(chCmdsArray[1]);
		int y = atoi(chCmdsArray[2]);

		if (!PointIsInMap(map, x, y))
		{
			printf("����Խ�硣\n");
			return;
		}

		switch (map->map[y][x].nObj)
		{
		case RS_NULL:
			printf("�����겢����Ʒ���顣\n");
			return;
			break;

		case RS_ROD:
			map->map[y][x].bPower = !map->map[y][x].bPower;
			break;

		case RS_BUTTON:
			ClickButton(map, x, y, p_bShowXY, *offset_x, *offset_y, *zoom);
			break;

		default:
			printf("����Ʒ��֧�ֵ��������\n");
			return;
			break;
		}
	}

	// resize �� ��
	else if (nArgsNum == 3 && strcmp(chCmdsArray[0], "resize") == 0 && isAllNum(chCmdsArray[1]) && isAllNum(chCmdsArray[2]))
	{
		int w = atoi(chCmdsArray[1]);
		int h = atoi(chCmdsArray[2]);

		if (w <= 0 || h <= 0)
		{
			printf("��ͼ��С����С�ڵ���0\n");
			return;
		}

		ReSizeRsMap(map, w, h);
	}

	// save ������Ŀ
	else if (nArgsNum == 1 && strcmp(chCmdsArray[0], "save") == 0)
	{
		if (SaveProject(*map, SelectFile(true)))
		{
			printf("����ɹ���\n");
			return;
		}
		else
		{
			printf("����ʧ�ܡ�\n");
			return;
		}
	}

	// ����ͼ�����xyƫ�ƶ��ص�0
	else if (nArgsNum == 1 && strcmp(chCmdsArray[0], "reset_map_offset") == 0)
	{
		*offset_x = 0;
		*offset_y = 0;
	}

	// �ƶ���ͼ
	else if (nArgsNum == 2 && strcmp(chCmdsArray[0], "up") == 0 && isAllNum(chCmdsArray[1]))
	{
		*offset_y -= atoi(chCmdsArray[1]) * offset_unit_size;
	}

	// �ƶ���ͼ
	else if (nArgsNum == 2 && strcmp(chCmdsArray[0], "down") == 0 && isAllNum(chCmdsArray[1]))
	{
		*offset_y += atoi(chCmdsArray[1]) * offset_unit_size;
	}

	// �ƶ���ͼ
	else if (nArgsNum == 2 && strcmp(chCmdsArray[0], "left") == 0 && isAllNum(chCmdsArray[1]))
	{
		*offset_x -= atoi(chCmdsArray[1]) * offset_unit_size;
	}

	// �ƶ���ͼ
	else if (nArgsNum == 2 && strcmp(chCmdsArray[0], "right") == 0 && isAllNum(chCmdsArray[1]))
	{
		*offset_x += atoi(chCmdsArray[1]) * offset_unit_size;
	}

	// ���ŵ�ͼ
	else if (nArgsNum == 2 && strcmp(chCmdsArray[0], "zoom") == 0 && isAllNum(chCmdsArray[1]))
	{
		*zoom = atoi(chCmdsArray[1]) / 100.0;
	}

	// ���ŵ�ͼ
	else if (nArgsNum == 2 && strcmp(chCmdsArray[0], "zoom+") == 0 && isAllNum(chCmdsArray[1]))
	{
		*zoom += atoi(chCmdsArray[1]) / 100.0;
	}

	// ���ŵ�ͼ
	else if (nArgsNum == 2 && strcmp(chCmdsArray[0], "zoom-") == 0 && isAllNum(chCmdsArray[1]))
	{
		*zoom -= atoi(chCmdsArray[1]) / 100.0;
	}

	// �õ���ͼƫ����
	else if (nArgsNum == 1 && strcmp(chCmdsArray[0], "get_map_offset") == 0)
	{
		printf("x��ƫ������%d����λ����%d���أ�\ny��ƫ������%d����λ����%d���أ�\n",
			*offset_x / offset_unit_size, *offset_x, *offset_y / offset_unit_size, *offset_y);
	}

	// �õ���ͼ������
	else if (nArgsNum == 1 && strcmp(chCmdsArray[0], "get_map_zoom") == 0)
	{
		printf("��������%d%%\n", (int)(*zoom * 100));
	}

	// import x y
	else if (nArgsNum == 3 && strcmp(chCmdsArray[0], "import") == 0 && isAllNum(chCmdsArray[1]) && isAllNum(chCmdsArray[2]))
	{
		int x = atoi(chCmdsArray[1]);
		int y = atoi(chCmdsArray[2]);

		if (!PointIsInMap(map, x, y))
		{
			printf("����Խ�硣\n");
			return;
		}

		printf("��ѡ��Ҫ�������Ŀ��\n");

		const WCHAR* map_file = SelectFile();
		RsMap import_map = OpenProject(map_file);

		char warning[512] = { 0 };
		sprintf_s(warning, 512,
			"�����ڵ���һ����Ŀ�������ǵ�����Ŀ����Ϣ��\n"
			"��������Ŀ������Ŀ�����Ͻ�λ�ã�( %d, %d )��\n"
			"����Ŀ��ͼ��С���� %d �񣬸� %d ��\n"
			"����Ŀ����������Ŀ�����·�Χ��( %d, %d ) �� ( %d, %d )�������������Ŀ�ĵ�ͼ��С���Զ��������Ĳ��ֲü���\n"
			"\n\n"
			"�Ƿ�����������Ŀ�������ȷ������ȷ���������Ŀ����������ȡ������ֹ�˲���������㵣����ɲ�����ĺ���������ȱ��浱ǰ��Ŀ���ٽ��е��롣\n"
			, x, y, import_map.w, import_map.h, x, y, x + import_map.w, y + import_map.h);

		wchar_t wstr[512] = { 0 };
		MultiByteToWideChar(CP_ACP, 0, warning, strlen(warning), wstr, strlen(warning));

		if (MessageBox(GetConsoleWindow(), wstr, L"ȷ����", MB_OKCANCEL) == IDOK)
		{
			ImportProject(map, import_map, x, y);
		}

		DeleteRsMap(&import_map);
		printf("������ϡ�\n");
	}

	// line ����x1 ����y1 ����x2 ����y2 (��Ʒ) : ����ʯֱ�ߣ������������������ĩβ������Ʒ����ʾ��ĳ�������ֱ�ߡ�
	else if ((nArgsNum == 5 || nArgsNum == 6) && strcmp(chCmdsArray[0], "line") == 0 &&
		isAllNum(chCmdsArray[1]) && isAllNum(chCmdsArray[2]) && isAllNum(chCmdsArray[3]) && isAllNum(chCmdsArray[4]))
	{
		int x1 = atoi(chCmdsArray[1]);
		int y1 = atoi(chCmdsArray[2]);
		int x2 = atoi(chCmdsArray[3]);
		int y2 = atoi(chCmdsArray[4]);
		int id = RS_POWDER;

		if (!PointIsInMap(map, x1, y1) || !PointIsInMap(map, x2, y2))
		{
			printf("����Խ�硣\n");
			return;
		}

		if (nArgsNum == 6)
		{
			int type;
			if (!GetIdFromString(chCmdsArray[5], &type, &id) || type != 0)
			{
				printf("��6������Ӧ������Ʒid��\n");
				return;
			}
		}

		if (!LineRsMap(map, x1, y1, x2, y2, id))
		{
			printf("(%d, %d) �� (%d, %d) ���������޷�����ֱ�ߡ�\n", x1, y1, x2, y2);
			return;
		}

	}

	// clear ����x1 ����y1 ����x2 ����y2 : �Կ������鸲�� (x1,y1) �� (x2,y2) �����з��顣
	else if (nArgsNum == 5 && strcmp(chCmdsArray[0], "clear") == 0 &&
		isAllNum(chCmdsArray[1]) && isAllNum(chCmdsArray[2]) && isAllNum(chCmdsArray[3]) && isAllNum(chCmdsArray[4]))
	{
		int x1 = atoi(chCmdsArray[1]);
		int y1 = atoi(chCmdsArray[2]);
		int x2 = atoi(chCmdsArray[3]);
		int y2 = atoi(chCmdsArray[4]);

		if (!PointIsInMap(map, x1, y1) || !PointIsInMap(map, x2, y2))
		{
			printf("����Խ�硣\n");
			return;
		}

		ClearRsMap(map, x1, y1, x2, y2);
	}

	else
	{
		printf("δ֪���\n");
		return;
	}

	printf("ִ����ϡ�\n");
}

int main()
{
	// ����ͼ��
	loadimages();

	// �õ���ʯ��ͼ
	RsMap map = StartMenu();

	system("cls");
	system("mode con cols=120 lines=40");
	printf("��ͼ���سɹ�������ָ���Բ�����ͼ������help�鿴������\n");

	initgraph(1024, 768, EW_SHOWCONSOLE);
	SetWindowPos(GetConsoleWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	// �Ƿ���ʾ��������
	bool bShowXY = true;

	// ��ͼƫ����ʾ
	int offset_x = 0;
	int offset_y = 0;

	// ��ͼ����
	double zoom = 1;

	// �����û�����
	while (true)
	{
		RunRsMap(&map);
		ProcessingImage(&map, &bShowXY, offset_x, offset_y, zoom);
		ProcessingCommand(&map, &bShowXY, &offset_x, &offset_y, &zoom);
	}

	closegraph();
	return 0;
}
