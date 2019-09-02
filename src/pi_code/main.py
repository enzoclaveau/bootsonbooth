import os
import serial
import time
from pygame import mixer, USEREVENT, event, display

ser = serial.Serial('/dev/ttyACM0', 250000)


class BootsOnBooth:
    music_folder = os.path.abspath(os.path.join(__file__, '..', '..', '..', '..', 'music'))
    print('Music folder: ' + music_folder)
    songs = list()
    number_of_songs = 0
    song_index = 11

    SONG_END = USEREVENT + 1

    TIME_NEXT_SONG = 10  # seconds
    NUMBER_OF_PLAYS = 3

    new_time = time.time()

    plays = 0

    timer_running = True

    def __init__(self):
        mixer.init(channels=2)
        display.init()
        mixer.music.set_endevent(self.SONG_END)
        self.load_song_paths()
        self.load_next_song()

    def calculate_next_song(self):
        print('calculating next song')
        if self.song_index < self.number_of_songs - 1:
            self.song_index = self.song_index + 1
        else:
            self.song_index = 0

    def load_song_paths(self):
        print('Loading songs')
        for subdir, dirs, files in os.walk(self.music_folder):
            for file in files:
                file = os.path.join(subdir, file)
                self.songs.append(file)
        self.number_of_songs = len(self.songs)
        print('Songs loaded')

    def play_music(self):
        print('play music')
        mixer.music.unpause()

    def load_next_song(self):
        print('loading_next_song')
        self.calculate_next_song()
        mixer.music.load(self.songs[self.song_index])
        mixer.music.play()
        self.stop_music()

    def play_next_song(self):
        self.load_next_song()
        print('start playing next_song: ' + str(self.song_index + 1) + '/' + str(self.number_of_songs))
        mixer.music.play()

    def song_has_end(self):
        for pygame_event in event.get():
            if pygame_event.type == self.SONG_END:
                print('song has end')
                return True

            return False

    def stop_music(self):
        print('stop music')
        mixer.music.pause()

        self.reset_timer()

    def time_has_elapsed(self):
        if self.elapsed_time() > self.TIME_NEXT_SONG:
            print('time has elapsed')
            self.reset_timer()
            return True
        return False

    def reset_timer(self):
        self.new_time = time.time()

    def start_timer(self):
        self.timer_running = True

    def stop_timer(self):
        self.timer_running = False

    def elapsed_time(self):
        if self.timer_running:
            elapsed_time = time.time() - self.new_time
            return elapsed_time
        else:
            return 0

    def sensor_input(self):
        if ser.in_waiting > 0:
            line = ser.readline().strip()
            print("Line: " + str(line))
            if line == '1':
                print("Boots are in position")
                self.increase_number_of_plays()
                return True
            elif line == '0':
                print("Boots are removed")
                return False

    def increase_number_of_plays(self):
        if self.plays < self.NUMBER_OF_PLAYS:
            self.plays = self.plays + 1
        else:
            self.plays = 0
            self.play_next_song()

        print("plays: " + str(self.plays))


def main():

    boots_on_booth = BootsOnBooth()

    while True:

        if boots_on_booth.song_has_end():
            boots_on_booth.play_next_song()

        if boots_on_booth.time_has_elapsed():
            boots_on_booth.load_next_song()

        sensor_input = boots_on_booth.sensor_input()
        if sensor_input is True:
            boots_on_booth.play_music()
            boots_on_booth.reset_timer()
            boots_on_booth.stop_timer()
        elif sensor_input is False:
            boots_on_booth.stop_music()
            boots_on_booth.start_timer()


if __name__ == "__main__":
    main()
