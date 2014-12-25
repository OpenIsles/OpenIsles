#ifndef _I_SOUND_MGR_H
#define _I_SOUND_MGR_H

/**
 * @brief Interface für den Sound-Manager, der sich um alle Sounds kümmert
 */
class ISoundMgr {

public:
    virtual ~ISoundMgr() {}

    /**
     * @brief aktiviert die Hintergrundmusik
     */
    virtual void enableMusic() = 0;

    /**
     * @brief deaktiviert die Hintergrundmusik
     */
    virtual void disableMusic() = 0;

};

#endif
