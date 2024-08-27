#pragma once
class CKeyMouse
{
public:
	HINSTANCE m_DLLInfo;
	CKeyMouse();
	CKeyMouse(CString strPath);
	~CKeyMouse();

	void MoveTo(int x,int y);
	void RightClick();  //ÓÒ¼üµ¥»÷
	void LeftClick();	//×ó¼üµ¥»÷
	void KeyPress(int Encodi);//µ¥»÷¼üÅÌ

	//×ó¼ü
	void LeftDown();
	void LeftUp();

	//ÓÒ¼ü
	void RightDown();
	void RightUp();

	void End();

};

