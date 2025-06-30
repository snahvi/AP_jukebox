#include "user.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>

// Playlist implementation
Playlist::Playlist()
{
}

Playlist::Playlist(const QString &name)
    : m_name(name)
{
}

QString Playlist::getName() const
{
    return m_name;
}

void Playlist::setName(const QString &name)
{
    m_name = name;
}

void Playlist::addSong(const Song &song)
{
    if (song.isValid()) {
        m_songs.append(song);
    }
}

void Playlist::removeSong(int index)
{
    if (index >= 0 && index < m_songs.size()) {
        m_songs.removeAt(index);
    }
}

void Playlist::clearPlaylist()
{
    m_songs.clear();
}

QList<Song> Playlist::getSongs() const
{
    return m_songs;
}

Song Playlist::getSong(int index) const
{
    if (index >= 0 && index < m_songs.size()) {
        return m_songs[index];
    }
    return Song(); // Return invalid song
}

int Playlist::getSongCount() const
{
    return m_songs.size();
}

bool Playlist::isEmpty() const
{
    return m_songs.isEmpty();
}

void Playlist::moveUp(int index)
{
    if (index > 0 && index < m_songs.size()) {
        m_songs.swapItemsAt(index, index - 1);
    }
}

void Playlist::moveDown(int index)
{
    if (index >= 0 && index < m_songs.size() - 1) {
        m_songs.swapItemsAt(index, index + 1);
    }
}

// User implementation
User::User()
{
}

User::User(const QString &username)
    : m_username(username)
{
}

QString User::getUsername() const
{
    return m_username;
}

void User::setUsername(const QString &username)
{
    m_username = username;
}

void User::createPlaylist(const QString &playlistName)
{
    if (!playlistName.isEmpty() && !m_playlists.contains(playlistName)) {
        m_playlists[playlistName] = Playlist(playlistName);
        qDebug() << "Created playlist:" << playlistName;
    }
}

void User::deletePlaylist(const QString &playlistName)
{
    if (m_playlists.contains(playlistName)) {
        m_playlists.remove(playlistName);
        
        // If we deleted the current playlist, clear the current playlist
        if (m_currentPlaylistName == playlistName) {
            m_currentPlaylistName.clear();
        }
        
        qDebug() << "Deleted playlist:" << playlistName;
    }
}

void User::renamePlaylist(const QString &oldName, const QString &newName)
{
    if (m_playlists.contains(oldName) && !newName.isEmpty()) {
        Playlist playlist = m_playlists[oldName];
        playlist.setName(newName);
        m_playlists.remove(oldName);
        m_playlists[newName] = playlist;
        
        // Update current playlist name if necessary
        if (m_currentPlaylistName == oldName) {
            m_currentPlaylistName = newName;
        }
        
        qDebug() << "Renamed playlist from" << oldName << "to" << newName;
    }
}

Playlist* User::getPlaylist(const QString &playlistName)
{
    if (m_playlists.contains(playlistName)) {
        return &m_playlists[playlistName];
    }
    return nullptr;
}

QStringList User::getPlaylistNames() const
{
    return m_playlists.keys();
}

int User::getPlaylistCount() const
{
    return m_playlists.size();
}

void User::setCurrentPlaylist(const QString &playlistName)
{
    if (m_playlists.contains(playlistName)) {
        m_currentPlaylistName = playlistName;
        qDebug() << "Set current playlist to:" << playlistName;
    }
}

Playlist* User::getCurrentPlaylist()
{
    if (!m_currentPlaylistName.isEmpty() && m_playlists.contains(m_currentPlaylistName)) {
        return &m_playlists[m_currentPlaylistName];
    }
    return nullptr;
}

QString User::getCurrentPlaylistName() const
{
    return m_currentPlaylistName;
}

QList<Song> User::scanFolderForSongs(const QString &folderPath)
{
    QList<Song> songs;
    QDir directory(folderPath);
    
    if (!directory.exists()) {
        qDebug() << "Folder does not exist:" << folderPath;
        return songs;
    }
    
    QStringList supportedFormats = getSupportedAudioFormats();
    QStringList filters;
    for (const QString &format : supportedFormats) {
        filters << "*." + format;
    }
    
    directory.setNameFilters(filters);
    directory.setFilter(QDir::Files);
    
    QFileInfoList fileList = directory.entryInfoList();
    for (const QFileInfo &fileInfo : fileList) {
        Song song(fileInfo.absoluteFilePath());
        if (song.isValid()) {
            songs.append(song);
        }
    }
    
    qDebug() << "Found" << songs.size() << "songs in folder:" << folderPath;
    return songs;
}

void User::createPlaylistFromFolder(const QString &playlistName, const QString &folderPath)
{
    createPlaylist(playlistName);
    Playlist* playlist = getPlaylist(playlistName);
    
    if (playlist) {
        QList<Song> songs = scanFolderForSongs(folderPath);
        for (const Song &song : songs) {
            playlist->addSong(song);
        }
        qDebug() << "Created playlist" << playlistName << "with" << songs.size() << "songs from folder:" << folderPath;
    }
}

QStringList User::getSupportedAudioFormats()
{
    return QStringList() << "mp3" << "wav" << "flac" << "m4a" << "aac" << "ogg" << "wma";
} 