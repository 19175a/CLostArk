#pragma once
class CKeyMouse
{
public:
	HINSTANCE m_DLLInfo;
	CKeyMouse();
	CKeyMouse(CString strPath);
	~CKeyMouse();

	void MoveTo(int x,int y);
	void RightClick();  //�Ҽ�����
	void LeftClick();	//�������
	void KeyPress(int Encodi);//��������

	//���
	void LeftDown();
	void LeftUp();

	//�Ҽ�
	void RightDown();
	void RightUp();

	void End();

};

