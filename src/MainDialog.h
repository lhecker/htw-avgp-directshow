#pragma once

#include "resource.h"

#include "afxcmn.h"
#include "afxwin.h"

#include "player.h"

class MainDialog : public CDialog {
public:
	MainDialog(CWnd* pParent = nullptr);

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum {
		IDD = IDD_HTWAVGP_DIALOG
	};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR timer);
	afx_msg void OnHScroll(UINT code, UINT pos, CScrollBar* scrollBar);
	afx_msg void OnLButtonUp(UINT code, CPoint pos);
	afx_msg void OnRButtonUp(UINT code, CPoint pos);
	afx_msg void OnBnClickedOpenButton();
	afx_msg LRESULT OnGraphNotify(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()

private:
	void layout_views();
	void layout_views(int cx, int cy);
	void update_position();

	std::optional<player> m_player;

	HICON m_hIcon;
	CButton m_open_button;
	CSliderCtrl m_slider;
	CStatic m_position;
};
