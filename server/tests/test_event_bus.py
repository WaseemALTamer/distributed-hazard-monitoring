import asyncio            # needed to run async code (for the last test)
import sys                # lets us change where Python looks for files
from pathlib import Path  # helps build folder paths

# Tell Python to also look in server/HttpServer, so it can find event_bus.py
sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "HttpServer"))

from event_bus import EventBus  # bring in the EventBus class we wrote


# TEST 1: does a subscriber get the message?
def test_subscriber_receives_data():
    bus = EventBus()                                # make a brand new, empty bus
    received = []                                   # empty list to collect messages
    bus.subscribe("test.channel", received.append)  # "add anything posted here to my list"
    bus.publish("test.channel", "hello")            # post "hello" to the channel
    assert received == ["hello"]                    # CHECK: the list must now hold "hello"


# TEST 2: do ALL subscribers get it, not just the first one?
def test_every_subscriber_receives_data():
    bus = EventBus()                                # new empty bus
    first, second = [], []                          # two empty lists
    bus.subscribe("test.channel", first.append)     # list 1 joins the channel
    bus.subscribe("test.channel", second.append)    # list 2 joins the same channel
    bus.publish("test.channel", 42)                 # post the number 42
    assert first == [42]                            # CHECK: list 1 got it
    assert second == [42]                           # CHECK: list 2 got it too


# TEST 3: are channels kept separate?
def test_channels_are_separate():
    bus = EventBus()                                # new empty bus
    received = []                                   # empty list
    bus.subscribe("channel.a", received.append)     # list joins channel A only
    bus.publish("channel.b", "not for you")         # post to channel B instead
    assert received == []                           # CHECK: list must still be empty


# TEST 4: posting to a channel nobody joined must not crash
def test_publish_with_no_subscribers_does_nothing():
    bus = EventBus()                                # new empty bus
    bus.publish("nobody.listening", "hello")        # post with no one listening
                                                    # (no check needed: passing = it didn't crash)


# TEST 5: if one subscriber crashes, do the others still get the message?
def test_crashing_subscriber_does_not_stop_others():
    bus = EventBus()                                # new empty bus
    received = []                                   # empty list

    def broken(data):                               # a function that always crashes
        raise RuntimeError("deliberate failure")    # by creating an error on purpose

    bus.subscribe("test.channel", broken)           # the crashing function joins FIRST
    bus.subscribe("test.channel", received.append)  # our list joins SECOND
    bus.publish("test.channel", "still delivered")  # post a message
    assert received == ["still delivered"]          # CHECK: list still got it despite the crash


# TEST 6: do async functions (like the web server's) get run?
def test_async_subscriber_runs():
    bus = EventBus()                                # new empty bus
    received = []                                   # empty list

    async def handler(data):                        # an async function (doesn't run instantly)
        received.append(data)                       # when it does run, add data to the list

    async def main():                               # async code must run inside an async function
        bus.subscribe("test.channel", handler)      # the async function joins the channel
        bus.publish("test.channel", "async hello")  # post a message
        await asyncio.sleep(0)                      # pause briefly so the async job gets to run

    asyncio.run(main())                             # start async mode and run main()
    assert received == ["async hello"]              # CHECK: the async function did run