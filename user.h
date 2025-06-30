#ifndef USER_H
#define USER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QDir>
#include "song.h"

class Playlist
{
public:
    Playlist();
    Playlist(const QString &name);
    
    QString getName() const;
    void setName(const QString &name);
    
    void addSong(const Song &song);
    bool containsSong(const Song &song) const;
    void removeSong(int index);
    void clearPlaylist();
    
    QList<Song> getSongs() const;
    Song getSong(int index) const;
    int getSongCount() const;
    bool isEmpty() const;
    
    void moveUp(int index);
    void moveDown(int index);
    

    
private:
    QString m_name;
    QList<Song> m_songs;
};

class Queue
{
public:
    Queue();
    
    void addSong(const Song &song);
    void insertSong(int position, const Song &song);
    void removeSong(int index);
    Song takeFirstSong(); // Removes and returns the first song
    void clearQueue();
    
    QList<Song> getSongs() const;
    Song getSong(int index) const;
    int getSongCount() const;
    bool isEmpty() const;
    
    void moveUp(int index);
    void moveDown(int index);
    
private:
    QList<Song> m_songs;
};

class User
{
public:
    User();
    User(const QString &username);
    
    // User info
    QString getUsername() const;
    void setUsername(const QString &username);
    
    // Playlist management
    void createPlaylist(const QString &playlistName);
    void deletePlaylist(const QString &playlistName);
    void renamePlaylist(const QString &oldName, const QString &newName);
    
    Playlist* getPlaylist(const QString &playlistName);
    QStringList getPlaylistNames() const;
    int getPlaylistCount() const;
    
    // Current playlist
    void setCurrentPlaylist(const QString &playlistName);
    Playlist* getCurrentPlaylist();
    QString getCurrentPlaylistName() const;
    
    // Queue management
    Queue* getQueue();
    void clearQueue();
    bool isQueueMode() const;
    void setQueueMode(bool enabled);
    
    // Simple shuffle and repeat
    bool isShuffleEnabled() const;
    void setShuffleEnabled(bool enabled);
    bool isRepeatEnabled() const;
    void setRepeatEnabled(bool enabled);
    Song getRandomSongFromCurrentPlaylist();
    
    // Folder scanning
    QList<Song> scanFolderForSongs(const QString &folderPath);
    void createPlaylistFromFolder(const QString &playlistName, const QString &folderPath);
    
    // Supported audio formats
    static QStringList getSupportedAudioFormats();
    
private:
    QString m_username;
    QMap<QString, Playlist> m_playlists;
    QString m_currentPlaylistName;
    Queue m_queue;
    bool m_queueMode;
    bool m_shuffleEnabled;
    bool m_repeatEnabled;
};

#endif // USER_H 