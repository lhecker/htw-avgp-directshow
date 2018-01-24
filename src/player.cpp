#include "stdafx.h"

#include "player.h"

namespace winrt {

template<>
inline constexpr GUID const& guid_of<IGraphBuilder>() noexcept {
	return IID_IGraphBuilder;
}

template<>
inline constexpr GUID const& guid_of<IMediaControl>() noexcept {
	return IID_IMediaControl;
}

template<>
inline constexpr GUID const& guid_of<IMediaSeeking>() noexcept {
	return IID_IMediaSeeking;
}

template<>
inline constexpr GUID const& guid_of<IVideoWindow>() noexcept {
	return IID_IVideoWindow;
}

template<>
inline constexpr GUID const& guid_of<IMediaEventEx>() noexcept {
	return IID_IMediaEventEx;
}

} // namespace winrt

const UINT WM_GRAPHNOTIFY = RegisterWindowMessageW(L"GRAPHNOTIFY");

player::player(HWND hwnd, const wchar_t* path) : m_hwnd(hwnd) {
	winrt::com_ptr<IGraphBuilder> graph;
	winrt::check_hresult(CoCreateInstance(__uuidof(FilgraphManager), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IGraphBuilder), graph.put_void()));

	m_media_control = graph.as<IMediaControl>();
	m_media_seeking = graph.as<IMediaSeeking>();
	m_window = graph.as<IVideoWindow>();
	m_event = graph.as<IMediaEventEx>();

	winrt::check_hresult(graph->RenderFile(path, nullptr));
	winrt::check_hresult(m_media_seeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME));
	winrt::check_hresult(m_event->SetNotifyWindow(OAHWND(m_hwnd), WM_GRAPHNOTIFY, 0));
	winrt::check_hresult(m_window->put_Owner(OAHWND(m_hwnd)));
	winrt::check_hresult(m_window->put_MessageDrain(OAHWND(m_hwnd)));
	winrt::check_hresult(m_window->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS));
	winrt::check_hresult(m_window->put_Visible(OATRUE));
}

player::~player() {
}

long player::get_next_event_code() {
	LONG code;
	LONG_PTR param1;
	LONG_PTR param2;

	if (m_event->GetEvent(&code, &param1, &param2, 0) == S_OK) {
		m_event->FreeEventParams(code, param1, param2);
		return code;
	}

	return {};
}

void player::set_window_position(long left, long top, long width, long height) {
	winrt::check_hresult(m_window->SetWindowPosition(left, top, width, height));
}

long long player::get_duration() const {
	long long duration;
	winrt::check_hresult(m_media_seeking->GetDuration(&duration));
	return duration;
}

long long player::get_position() const {
	long long position;
	winrt::check_hresult(m_media_seeking->GetCurrentPosition(&position));
	return position;
}

void player::set_position(long long position) {
	winrt::check_hresult(m_media_seeking->SetPositions(&position, AM_SEEKING_AbsolutePositioning, nullptr, AM_SEEKING_NoPositioning));
}

bool player::is_paused() const {
	return m_paused;
}

void player::set_paused(bool paused) {
	if (paused == m_paused) {
		return;
	}

	if (paused) {
		winrt::check_hresult(m_media_control->Pause());
	} else {
		winrt::check_hresult(m_media_control->Run());
	}

	m_paused = paused;
}

bool player::is_fullscreen() const {
	return m_fullscreen;
}

void player::set_fullscreen(bool fullscreen) {
	if (fullscreen == m_fullscreen) {
		return;
	}

	winrt::check_hresult(m_window->put_FullScreenMode(fullscreen ? OATRUE : OAFALSE));
	m_fullscreen = fullscreen;
}
