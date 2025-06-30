#ifndef SONG_H
#define SONG_H

#include <QString>
#include <QFileInfo>

class Song
{
public:
    Song();
    Song(const QString &filePath);
    
    // Getters
    QString getFilePath() const;
    QString getTitle() const;
    QString getArtist() const;  
    QString getAlbum() const;
    QString getFileName() const;
    qint64 getDuration() const;
    bool isValid() const;
    
    // Setters
    void setFilePath(const QString &filePath);
    void setTitle(const QString &title);
    void setArtist(const QString &artist);
    void setAlbum(const QString &album);
    void setDuration(qint64 duration);
    
    // Helper methods
    void loadMetadata();
    QString getDisplayName() const;
    
private:
    QString m_filePath;
    QString m_title;
    QString m_artist;
    QString m_album;
    QString m_fileName;
    qint64 m_duration;
    bool m_valid;
};

#endif // SONG_H 