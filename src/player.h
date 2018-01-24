#pragma once

extern const UINT WM_GRAPHNOTIFY;

class player {
public:
	player(HWND hwnd, const wchar_t* path);
	~player();

	long get_next_event_code();
	void set_window_position(long left, long top, long width, long height);

	long long get_duration() const;

	long long get_position() const;
	void set_position(long long position);

	bool is_paused() const;
	void set_paused(bool paused);

	bool is_fullscreen() const;
	void set_fullscreen(bool fullscreen);

private:
	winrt::com_ptr<IMediaControl> m_media_control;
	winrt::com_ptr<IMediaSeeking> m_media_seeking;
	winrt::com_ptr<IVideoWindow> m_window;
	winrt::com_ptr<IMediaEventEx> m_event;
	HWND m_hwnd;
	bool m_fullscreen = false;
	bool m_paused = true;
};

