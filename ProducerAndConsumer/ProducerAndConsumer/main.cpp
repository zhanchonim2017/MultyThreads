//1������ 1������ 1������  
//ʹ�ö����¼���һ����ʾ�������գ�һ����ʾ����������  
//��ʹ��һ���ؼ��������ƻ������ķ���  

#include <stdio.h>
#include <process.h>
#include <Windows.h>

// ���ÿ���̨����ɫ
BOOL SetConsoleColor(WORD wAttributes)  
{  
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  
    if (hConsole == INVALID_HANDLE_VALUE)  
        return FALSE;     
    return SetConsoleTextAttribute(hConsole, wAttributes);  
}

const int END_PRODUCE_NUMBER = 10;		// ������Ʒ����
int g_buffer;		// ������
CRITICAL_SECTION g_cs;		// �ٽ���
HANDLE g_hEventBufferEmpty, g_hEventBufferFull;	// �����¼������Ϳ�

// �������̺߳���
unsigned int __stdcall ProducerThreadFun(PVOID pM)
{
	for (int i = 1; i <= END_PRODUCE_NUMBER; i++)
	{
		// �ȴ�������Ϊ��
		WaitForSingleObject(g_hEventBufferEmpty, INFINITE);

		// ����ķ��ʻ�����
		EnterCriticalSection(&g_cs);
		g_buffer = i;
		printf("�����߽�����%d���뻺����\n", g_buffer);
		LeaveCriticalSection(&g_cs);

		// ����������Ϊ���¼�
		SetEvent(g_hEventBufferFull);
	}

	return 0;
}


// �������̺߳���
unsigned int __stdcall ConsumerThreadFun(PVOID pM)
{
	volatile bool flag = true;
	while (flag)
	{
		// �ȴ��̳߳�Ϊ��
		WaitForSingleObject(g_hEventBufferFull, INFINITE);

		// ����ķ��ʻ�����
		EnterCriticalSection(&g_cs);
		SetConsoleColor(FOREGROUND_GREEN);
		printf("	�����ߴӻ�������ȡ����%d\n", g_buffer);
		SetConsoleColor(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
		
		if (END_PRODUCE_NUMBER == g_buffer)
		{
			flag = false;
		}
		LeaveCriticalSection(&g_cs);

		// ����������Ϊ���¼�
		SetEvent(g_hEventBufferEmpty);

		Sleep(10);		// some other work to do;
	}

	return 0;
}

int main()
{
	printf("  ����������������   1������ 1������ 1������\n");  

	InitializeCriticalSection(&g_cs);
	//���������Զ���λ�¼���һ����ʾ�������Ƿ�Ϊ�գ���һ����ʾ�������Ƿ��Ѿ�����  
	g_hEventBufferEmpty = CreateEvent(NULL, FALSE, TRUE, NULL);
	g_hEventBufferFull	= CreateEvent(NULL, FALSE, TRUE, NULL);

	const int THREADNUM = 2;
	HANDLE hThread[THREADNUM];

	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, ProducerThreadFun, NULL, 0, NULL);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, ConsumerThreadFun, NULL, 0, NULL);
	WaitForMultipleObjects(THREADNUM, hThread, true, INFINITE);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);

	// �����¼����ٽ���
	CloseHandle(g_hEventBufferEmpty);
	CloseHandle(g_hEventBufferFull);
	DeleteCriticalSection(&g_cs);

	return 0;
}