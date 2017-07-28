// sysinfo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <tchar.h>

#define DIV 1024
#define WIDTH 7
#define GB(x) (((x).HighPart << 2) + ((DWORD)(x).LowPart) / 1024.0 / 1024.0 / 1024.0)

#pragma warning(disable: 4996)   

#define true 1
typedef	unsigned char U_CHAR;

//��ȡϵͳʱ��
void getSystemTime(char* buffer){

	char buf[128];
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buf, sizeof(buf), "Now is %Y-%m-%d %H:%M:%S", timeinfo);
	strcpy(buffer, buf);
}

//CPU������
FILETIME m_preidleTime;
FILETIME m_prekernelTime;
FILETIME m_preuserTime;

__int64 CompareFileTime2(FILETIME time1, FILETIME time2)
{
	__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime;
	__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime;


	return   b - a;
}

void GetCpuUseage()
{
	GetSystemTimes(&m_preidleTime, &m_prekernelTime, &m_preuserTime);
	Sleep(1000);
}

double CpuUseage()
{
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;
	GetSystemTimes(&idleTime, &kernelTime, &userTime);


	int idle = CompareFileTime2(m_preidleTime, idleTime);
	int kernel = CompareFileTime2(m_prekernelTime, kernelTime);
	int user = CompareFileTime2(m_preuserTime, userTime);


	if (kernel + user == 0)
		return 0.0;
	//���ܵ�ʱ��-����ʱ�䣩/�ܵ�ʱ��=ռ��cpu��ʱ�����ʹ����
	double cpu = abs((kernel + user - idle) * 100 / (kernel + user));

	m_preidleTime = idleTime;
	m_prekernelTime = kernelTime;
	m_preuserTime = userTime;
	return cpu;
}

//��ȡ����ϵͳ������
void GetSystemName(char* osname)
{
	char buf[128];
	SYSTEM_INFO info;        //��SYSTEM_INFO�ṹ�ж�64λAMD������ 
	GetSystemInfo(&info);    //����GetSystemInfo�������ṹ 
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	strcpy(buf, "unknown OperatingSystem.");

	if (GetVersionEx((OSVERSIONINFO *)&os))
	{
		//������ݰ汾��Ϣ�жϲ���ϵͳ���� 
		switch (os.dwMajorVersion)//�ж����汾��
		{
		case 4:
			switch (os.dwMinorVersion)//�жϴΰ汾�� 
			{
			case 0:
				if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
					strcpy(buf, "Microsoft Windows NT 4.0"); //1996��7�·��� 
				else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
					strcpy(buf, "Microsoft Windows 95");
				break;
			case 10:
				strcpy(buf, "Microsoft Windows 98");
				break;
			case 90:
				strcpy(buf, "Microsoft Windows Me");
				break;
			}
			break;

		case 5:
			switch (os.dwMinorVersion)	//�ٱȽ�dwMinorVersion��ֵ
			{
			case 0:
				strcpy(buf, "Microsoft Windows 2000"); //1999��12�·���
				break;

			case 1:
				strcpy(buf, "Microsoft Windows XP"); //2001��8�·���
				break;

			case 2:
				if (os.wProductType == VER_NT_WORKSTATION
					&& info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				{
					strcpy(buf, "Microsoft Windows XP Professional x64 Edition");
				}
				else if (GetSystemMetrics(SM_SERVERR2) == 0)
					strcpy(buf, "Microsoft Windows Server 2003"); //2003��3�·��� 
				else if (GetSystemMetrics(SM_SERVERR2) != 0)
					strcpy(buf, "Microsoft Windows Server 2003 R2");
				break;
			}
			break;

		case 6:
			switch (os.dwMinorVersion)
			{
			case 0:
				if (os.wProductType == VER_NT_WORKSTATION)
					strcpy(buf, "Microsoft Windows Vista");
				else
					strcpy(buf, "Microsoft Windows Server 2008"); //�������汾 
				break;
			case 1:
				if (os.wProductType == VER_NT_WORKSTATION)
					strcpy(buf, "Microsoft Windows 7");
				else
					strcpy(buf, "Microsoft Windows Server 2008 R2");
				break;
			}
			break;
		}
	}//if(GetVersionEx((OSVERSIONINFO *)&os))

	strcpy(osname, buf);
}

int main(){

	//���ϵͳʱ��
	char buffer[128];
	getSystemTime(buffer);
	puts(buffer);
	/*
	//ϵͳ����ʱ��
	clock_t start, finish;
	double duration;

	start = clock();

	
	while (true)
	{
		finish = clock();
		duration = (double)(finish - start) / CLOCKS_PER_SEC;
		Sleep(1000);
		printf("%.0f ��\n", duration);

	}
	*/
	//ϵͳ�汾����
	char osname[20];
	GetSystemName(osname);
	printf("%s\n", osname);
	
	//CPU������
	while (true)
	{
		U_CHAR  sysStateCpu[5];
		double cpu = CpuUseage();
		sprintf((char*)sysStateCpu, "%.2lf", cpu);
		printf("CPUʹ���ʣ�%s%%\n", sysStateCpu);
		Sleep(1000);
	}
	
	//�ڴ�ʹ����
	char bufPreCPU[5];
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	sprintf(bufPreCPU, "%ld%%\n", statex.dwMemoryLoad);
	puts(bufPreCPU);
   
	//Ӳ��ʹ���� ����windows API
	ULARGE_INTEGER FreeBytesAvailableC, TotalNumberOfBytesC, TotalNumberOfFreeBytesC;
	ULARGE_INTEGER FreeBytesAvailableD, TotalNumberOfBytesD, TotalNumberOfFreeBytesD;
	ULARGE_INTEGER FreeBytesAvailableE, TotalNumberOfBytesE, TotalNumberOfFreeBytesE;
	ULARGE_INTEGER FreeBytesAvailableF, TotalNumberOfBytesF, TotalNumberOfFreeBytesF;

	GetDiskFreeSpaceEx(_T("C:"), &FreeBytesAvailableC, &TotalNumberOfBytesC, &TotalNumberOfFreeBytesC);
	GetDiskFreeSpaceEx(_T("D:"), &FreeBytesAvailableD, &TotalNumberOfBytesD, &TotalNumberOfFreeBytesD);
	GetDiskFreeSpaceEx(_T("E:"), &FreeBytesAvailableE, &TotalNumberOfBytesE, &TotalNumberOfFreeBytesE);
	GetDiskFreeSpaceEx(_T("F:"), &FreeBytesAvailableF, &TotalNumberOfBytesF, &TotalNumberOfFreeBytesF);
	/*���� ���ͼ�˵��
	lpRootPathName String�������������Ĵ��̸�·����
	lpFreeBytesAvailableToCaller LARGE_INTEGER��ָ��һ���������������ɵ����߿��õ��ֽ�����
	lpTotalNumberOfBytes LARGE_INTEGER��ָ��һ���������������ɴ����ϵ����ֽ���
	lpTotalNumberOfFreeBytes LARGE_INTEGER��ָ��һ���������������ɴ����Ͽ��õ��ֽ���
	����ƽ̨
	Windows 95 OSR2��Windows NT 4.0*/
	float totalHardDisk = GB(TotalNumberOfBytesC) + GB(TotalNumberOfBytesD) + GB(TotalNumberOfBytesE) + GB(TotalNumberOfBytesF);
	float freeHardDisk = GB(TotalNumberOfFreeBytesC) + GB(TotalNumberOfFreeBytesD) + GB(TotalNumberOfFreeBytesE) + GB(TotalNumberOfFreeBytesF);

	float hardDiskUsage = 1 - freeHardDisk / totalHardDisk;

	//CPU����ת��

	printf("hello world��\n");
	system("pause");

}


/*
int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}
*/

