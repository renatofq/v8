#include <v8/dispatcher.h>
#include <v8/map.h>
#include <v8/log.h>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>

#define V8_MAX_EVENTS (10)

struct v8_dispatcher_t
{
	int run;
	int epoll;
	V8Map * event_map;
};

typedef struct v8_epoll_event_t
{
	int fd;
	V8Listener listener;
	struct epoll_event ev;
} V8EpollEvent;


static V8EpollEvent * v8_epoll_event_create(int fd, const V8Listener * listener);
static void v8_epoll_event_destroy(V8EpollEvent * epev);
static uint32_t v8_epoll_setup_bitset(const V8Listener * listener);
static void v8_epoll_call(int fd, uint32_t events, const V8Listener * listener);


V8Dispatcher * v8_dispatcher_create(void)
{
	V8Dispatcher * dispatcher = malloc(sizeof(V8Dispatcher));

	if (dispatcher != NULL)
	{
		dispatcher->run = 0;

		/* size parameter is deprecated since Linux 2.6.8 */
		dispatcher->epoll = epoll_create(1);

		/* FIXME: The follwing cast is potentially bad */
		dispatcher->event_map = v8_map_create(NULL, (V8MapDestructor) v8_epoll_event_destroy);

		if (dispatcher->epoll < 0 || dispatcher->event_map == NULL)
		{
			v8_dispatcher_destroy(dispatcher);
			dispatcher = NULL;
		}
	}

	return dispatcher;
}

void v8_dispatcher_destroy(V8Dispatcher * dispatcher)
{
	if (dispatcher == NULL)
	{
		return;
	}

	if (dispatcher->epoll > 0)
	{
		close(dispatcher->epoll);
		dispatcher->epoll = -1;
	}


	if (dispatcher->event_map != NULL)
	{
		v8_map_destroy(dispatcher->event_map);
		dispatcher->event_map = NULL;
	}

	free(dispatcher);
	dispatcher = NULL;
}

int v8_dispatcher_add_listener(V8Dispatcher * dispatcher, int fd,
                               const V8Listener * listener)
{
	V8EpollEvent * epev = NULL;
	int ret = -1;


	if (dispatcher == NULL || fd < 0 || listener == NULL)
	{
		return ret;
	}

	epev = v8_epoll_event_create(fd, listener);
	if (epev == NULL)
	{
		return ret;
	}

	if (v8_map_insert(dispatcher->event_map, (v8_key_t) fd, epev) == 1)
	{
		ret = epoll_ctl(dispatcher->epoll, EPOLL_CTL_ADD, fd, &epev->ev);
		if (ret == -1)
		{
			v8_log_error("Error while adding fd to epoll: %d", errno);
		}
	}

	return ret;
}

void v8_dispatcher_start(V8Dispatcher * dispatcher)
{
	struct epoll_event events[V8_MAX_EVENTS];
	const V8EpollEvent * curr_event = NULL;
	const V8Map * event_map = NULL;
	int nfds;
	int i;

	if (dispatcher == NULL)
	{
		return;
	}

	dispatcher->run = 1;

	event_map = dispatcher->event_map;

	while (dispatcher->run)
	{
		v8_log_debug("Waiting events");
		nfds = epoll_wait(dispatcher->epoll, events, V8_MAX_EVENTS, -1);
		if (nfds == -1)
		{
			v8_log_error("Error waiting events");
		}

		for (i = 0; i < nfds; ++i)
		{
			curr_event = v8_map_value(event_map, (v8_key_t) events[i].data.fd);
			if (curr_event != NULL)
			{
				v8_epoll_call(curr_event->fd, events[i].events, &curr_event->listener);
			}
			else
			{
				v8_log_warn("File descriptor not found on event map");
			}
		}
	}
}

void v8_dispatcher_stop(V8Dispatcher * dispatcher)
{
	if (dispatcher == NULL)
	{
		v8_log_error("Null dispacther");
		return;
	}

	dispatcher->run = 0;
}

static V8EpollEvent * v8_epoll_event_create(int fd, const V8Listener * listener)
{
	V8EpollEvent * epev = malloc(sizeof(V8EpollEvent));

	if (epev != NULL)
	{
		memset(epev, 0, sizeof(V8EpollEvent));
		epev->fd = fd;
		epev->listener = *listener;
		epev->ev.events = v8_epoll_setup_bitset(listener);
		epev->ev.data.fd = fd;
	}

	return epev;
}

static uint32_t v8_epoll_setup_bitset(const V8Listener * listener)
{
	uint32_t event_bitset = 0;

	if (listener->input_handler != NULL)
	{
		event_bitset |= EPOLLIN;
	}

	if (listener->output_handler != NULL)
	{
		event_bitset |= EPOLLOUT;
	}

	if (listener->closed_handler != NULL)
	{
		event_bitset |= EPOLLRDHUP;
	}

	event_bitset |= EPOLLET;

	return event_bitset;
}

static void v8_epoll_event_destroy(V8EpollEvent * epev)
{
	if (epev == NULL)
	{
		return;
	}

	V8Listener listener = epev->listener;

	if (listener.destructor != NULL && listener.data != NULL)
	{
		listener.destructor(listener.data);
		listener.data = NULL;
	}

	memset(&epev->listener, 0, sizeof(V8Listener));


	free(epev);
}

static void v8_epoll_call(int fd, uint32_t events, const V8Listener * listener)
{
	if (events & EPOLLIN)
	{
		listener->input_handler(fd, listener->data);
	}
	else if (events & EPOLLOUT)
	{
		listener->output_handler(fd, listener->data);
	}
	else if (events & EPOLLRDHUP)
	{
		listener->closed_handler(fd, listener->data);
	}
	else if (events & EPOLLERR)
	{
		listener->error_handler(fd, listener->data);
	}
	else if (events & EPOLLHUP)
	{
		listener->hangup_handler(fd, listener->data);
	}

}

#undef V8_MAX_EVENTS
