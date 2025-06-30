#include "song.h"
#include <QFileInfo>
#include <QDir>

Song::Song()
    : m_duration(0), m_valid(false)
{
}

Song::Song(const QString &filePath)
    : m_filePath(filePath), m_duration(0), m_valid(false)
{
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists() && fileInfo.isFile()) {
        m_fileName = fileInfo.fileName();
        m_valid = true;
        loadMetadata();
    }
}

QString Song::getFilePath() const
{
    return m_filePath;
}

QString Song::getTitle() const
{
    return m_title.isEmpty() ? m_fileName : m_title;
}

QString Song::getArtist() const
{
    return m_artist;
}

QString Song::getAlbum() const
{
    return m_album;
}

QString Song::getFileName() const
{
    return m_fileName;
}

qint64 Song::getDuration() const
{
    return m_duration;
}

bool Song::isValid() const
{
    return m_valid;
}

void Song::setFilePath(const QString &filePath)
{
    m_filePath = filePath;
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists() && fileInfo.isFile()) {
        m_fileName = fileInfo.fileName();
        m_valid = true;
        loadMetadata();
    } else {
        m_valid = false;
    }
}

void Song::setTitle(const QString &title)
{
    m_title = title;
}

void Song::setArtist(const QString &artist)
{
    m_artist = artist;
}

void Song::setAlbum(const QString &album)
{
    m_album = album;
}

void Song::setDuration(qint64 duration)
{
    m_duration = duration;
}

void Song::loadMetadata()
{
    // Basic metadata extraction from filename
    // In a real application, you'd use a library like TagLib for proper metadata extraction
    QFileInfo fileInfo(m_filePath);
    QString baseName = fileInfo.baseName();
    
    // Try to extract artist and title from filename patterns like "Artist - Title"
    if (baseName.contains(" - ")) {
        QStringList parts = baseName.split(" - ", Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            m_artist = parts[0].trimmed();
            m_title = parts[1].trimmed();
        }
    }
    
    // If no title was extracted, use the filename
    if (m_title.isEmpty()) {
        m_title = baseName;
    }
}

QString Song::getDisplayName() const
{
    if (!m_artist.isEmpty() && !m_title.isEmpty()) {
        return QString("%1 - %2").arg(m_artist, m_title);
    } else if (!m_title.isEmpty()) {
        return m_title;
    } else {
        return m_fileName;
    }
} 