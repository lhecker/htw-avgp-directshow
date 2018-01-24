#include "stdafx.h"

#include "MainDialog.h"

#include "MainApp.h"

static const UINT_PTR init_timer_id = UINT_PTR(&init_timer_id);
static const UINT_PTR position_timer_id = UINT_PTR(&position_timer_id);

BEGIN_MESSAGE_MAP(MainDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_OPEN_BUTTON, &MainDialog::OnBnClickedOpenButton)
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_REGISTERED_MESSAGE(WM_GRAPHNOTIFY, &MainDialog::OnGraphNotify)
END_MESSAGE_MAP()

MainDialog::MainDialog(CWnd* pParent) : CDialog(IDD_HTWAVGP_DIALOG, pParent) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void MainDialog::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPEN_BUTTON, m_open_button);
	DDX_Control(pDX, IDC_SLIDER, m_slider);
	DDX_Control(pDX, IDC_POSITION, m_position);
}

BOOL MainDialog::OnInitDialog() {
	CDialog::OnInitDialog();

	// Set the icon for this dialog. The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE); // Set big icon
	SetIcon(m_hIcon, FALSE); // Set small icon

	m_slider.SetRangeMin(0);
	m_slider.SetTicFreq(1);

	SetTimer(init_timer_id, 1, nullptr);

	return TRUE; // return TRUE unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
// to draw the icon. For MFC applications using the document/view model,
// this is automatically done for you by the framework.
void MainDialog::OnPaint() {
	if (!IsIconic()) {
		CDialog::OnPaint();
		return;
	}

	CPaintDC dc(this); // device context for painting

	SendMessageW(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.m_hDC), 0);

	// Center icon in client rectangle
	int cxIcon = GetSystemMetrics(SM_CXICON);
	int cyIcon = GetSystemMetrics(SM_CYICON);
	CRect rect;
	GetClientRect(&rect);
	int x = (rect.Width() - cxIcon + 1) / 2;
	int y = (rect.Height() - cyIcon + 1) / 2;

	// Draw the icon
	dc.DrawIcon(x, y, m_hIcon);
}

// The system calls this function to obtain the cursor to
// display while the user drags the minimized window.
HCURSOR MainDialog::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}

void MainDialog::OnSize(UINT type, int cx, int cy) {
	CDialog::OnSize(type, cx, cy);

	if (m_slider) {
		layout_views(cx, cy);
	}
}

void MainDialog::OnTimer(UINT_PTR timer) {
	if (timer == init_timer_id) {
		KillTimer(timer);
		layout_views();
		return;
	}
	if (timer == position_timer_id) {
		if (m_player) {
			update_position();
		} else {
			KillTimer(position_timer_id);
		}
		return;
	}
}

void MainDialog::OnHScroll(UINT code, UINT pos, CScrollBar* scrollBar) {
	UNREFERENCED_PARAMETER(code);
	UNREFERENCED_PARAMETER(pos);

	if (scrollBar->GetDlgCtrlID() != IDC_SLIDER) {
		return;
	}
	if (!m_player.has_value()) {
		return;
	}

	m_player->set_position(m_slider.GetPos() * 10000000LL);
}

void MainDialog::OnLButtonUp(UINT code, CPoint pos) {
	UNREFERENCED_PARAMETER(code);
	UNREFERENCED_PARAMETER(pos);

	if (m_player.has_value()) {
		m_player->set_fullscreen(!m_player->is_fullscreen());
	}
}

void MainDialog::OnRButtonUp(UINT code, CPoint pos) {
	UNREFERENCED_PARAMETER(code);
	UNREFERENCED_PARAMETER(pos);

	if (m_player.has_value()) {
		m_player->set_paused(!m_player->is_paused());
	}
}

void MainDialog::OnBnClickedOpenButton() {
	CFileDialog dlg(TRUE, nullptr, nullptr, OFN_FILEMUSTEXIST, L"Video-Files (*.avi, *.mov, *.mp4, *.wmv)|*.avi;*.mov;*.mp4;*.wmv|All Files (*.*)|*.*||");
	if (dlg.DoModal() != IDOK) {
		return;
	}

	m_player = player(GetSafeHwnd(), dlg.GetPathName());

	m_slider.SetRangeMax(int(m_player->get_duration() / 10000000LL));
	m_slider.SetPos(0);

	layout_views();
	SetTimer(position_timer_id, 151, nullptr);
}

LRESULT MainDialog::OnGraphNotify(WPARAM wparam, LPARAM lparam) {
	UNREFERENCED_PARAMETER(wparam);
	UNREFERENCED_PARAMETER(lparam);

	if (!m_player.has_value()) {
		return 0;
	}

	while (true) {
		const auto code = m_player->get_next_event_code();
		if (code == 0) {
			break;
		}

		switch (code) {
		case EC_COMPLETE:
			m_player->set_paused(true);
			m_player->set_fullscreen(false);
			m_player->set_position(0);
			break;
		case EC_USERABORT:
			m_player.reset();
			return 0;
		}
	}

	return 0;
}

void MainDialog::layout_views() {
	CRect rect;
	GetClientRect(&rect);
	layout_views(rect.Width(), rect.Height());
}

void MainDialog::layout_views(int cx, int cy) {
	constexpr int padding = 6;
	constexpr int slider_position_padding = 2 * padding;
	constexpr int height = 32;
	constexpr int open_button_width = 64;
	constexpr int position_width = 48;

	const int toolbar_y = cy - padding - height;

	const int open_button_x = padding;
	const int slider_x = open_button_x + open_button_width + padding;
	const int position_x = cx - padding - position_width;

	const int slider_width = position_x - padding - slider_x;

	m_open_button.SetWindowPos(
		nullptr,
		open_button_x,
		toolbar_y,
		open_button_width,
		height,
		0
	);
	m_slider.SetWindowPos(
		nullptr,
		slider_x,
		toolbar_y,
		slider_width,
		height,
		0
	);
	m_position.SetWindowPos(
		nullptr,
		position_x,
		toolbar_y,
		position_width,
		height,
		0
	);

	if (m_player.has_value()) {
		m_player->set_window_position(0, 0, cx, toolbar_y - padding);
	}
}

void MainDialog::update_position() {
	const auto position = m_player->get_position() / 10000000LL;

	CString position_str;
	position_str.Format(L"%02lld:%02lld", position / 60, position % 60);

	m_slider.SetPos(int(position));
	m_position.SetWindowTextW(position_str);
}
