import asyncio      #Python’s toolkit for async code. FastAPI uses it.
import inspect      #lets us check what kind of thing a function returned.
import logging      #prints error messages properly, with timestamps, instead of using print.

logger = logging.getLogger(__name__)    #Creates a logger named after this file (event_bus), so error messages show where they came from.


#The class and its storage
class EventBus:
    """A simple publish/subscribe channel system for the hub."""

    def __init__(self):
        self._subscribers = {}  #_subscribers is a dictionary of channels. Each key is a channel name, and each value is the list of functions that joined it.
        self._tasks = set()     #_tasks holds async jobs that are still running.

    #Joining a Channel
    def subscribe(self, channel, handler):
        self._subscribers.setdefault(channel, []).append(handler)
        #setdefault(channel, []) -> : “get this channel’s list, or create an empty one if the channel doesn’t exist yet.”
        #.append(handler) -> adds the function to that list.



    #Posting to a channel
    def publish(self, channel, data=None):
        for handler in list(self._subscribers.get(channel, [])): 
        #.get(channel, []) gets the channel’s list of functions, or an empty list if nobody joined. Publishing to an empty channel is fine.
        # list() makes a copy of the list first. If a handler subscribes something new while we’re looping, it won’t break the loop.    
            try: #starts safety rule 1: if this handler crashes, we catch it below.
                result = handler(data) #Calls each function with the data. This is the moment the message gets “delivered”.
                if inspect.isawaitable(result): #checks whether we got one of those jobs back. 'isawaitable' checks whether we got one of those jobs back.
                    task = asyncio.get_running_loop().create_task(result) #safety rule 2, which handles async functions.
                #'create_task' hands the job to the event loop (FastAPI’s engine) to run in the background. publish doesn’t wait for it, so the UDP listener is never held up.
                #Note: An async def function doesn’t run when you call it. It hands back a coroutine, a “job to be run”, and something has to actually run it.
                    self._tasks.add(task)
                    task.add_done_callback(self._task_finished) #add_done_callback -> “when this job finishes, call _task_finished”.
            except Exception:
                logger.exception("Handler %r failed on channel %r", handler, channel) #If a normal handler crashed, we log the error, including which handler and which channel, and carry on with the next handler - safety rule 1 

    def _task_finished(self, task):
        self._tasks.discard(task)   #Removes the finished job from _tasks.
        if not task.cancelled() and task.exception() is not None:
            logger.error("Async handler failed", exc_info=task.exception()) #If the async job crashed, its error would otherwise vanish silently, so log it.

bus = EventBus() #Creates one bus for the whole hub. Every file does from event_bus import bus, so everyone uses the same channels.