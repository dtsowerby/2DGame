const audioCache = new Map();
const loopingAudio = new Map();
let userHasInteracted = false;
const pendingAudio = [];
const pendingLoopedAudio = new Map();

function getAudio(filename) {
  let normalizedPath = filename;
  if (normalizedPath.startsWith('/')) {
    normalizedPath = normalizedPath.substring(1);
  }
  const pathVariations = [
    normalizedPath,
    './' + normalizedPath,
    filename,
    filename.replace('res-web/', './res-web/'),
    filename.replace('res-web/', '')
  ];
  
  if (!audioCache.has(filename)) {
    const audio = new Audio(pathVariations[0]);
    audio.preload = 'auto';
    
    audio.addEventListener('error', (e) => {
      console.error('Audio error for path:', pathVariations[0], e);
    });
    
    audioCache.set(filename, audio);
  }
  return audioCache.get(filename);
}

function enableAudio() {
  if (userHasInteracted) return;
  
  userHasInteracted = true;
  
  while (pendingAudio.length > 0) {
    const pendingAction = pendingAudio.shift();
    try {
      pendingAction();
    } catch (err) {
      console.warn('Failed to play pending audio:', err);
    }
  }
  
  pendingLoopedAudio.forEach((pendingAction, filename) => {
    try {
      pendingAction();
    } catch (err) {
      console.warn('Failed to play pending looped audio:', err);
    }
  });
  
  pendingLoopedAudio.clear();
}

function setupInteractionListeners() {
  const events = ['click', 'touchstart', 'keydown', 'mousedown'];
  
  const handleInteraction = () => {
    enableAudio();
    events.forEach(event => {
      document.removeEventListener(event, handleInteraction, true);
    });
  };
  
  events.forEach(event => {
    document.addEventListener(event, handleInteraction, true);
  });
}

if (document.readyState === 'loading') {
  document.addEventListener('DOMContentLoaded', setupInteractionListeners);
} else {
  setupInteractionListeners();
}

function playAudioSafely(audio, onError) {
  if (userHasInteracted) {
    return audio.play().catch((err) => {
      console.error('Failed to play audio:', err);
      onError(err);
    });
  } else {
    pendingAudio.push(() => {
      return audio.play().catch((err) => {
        onError(err);
      });
    });
    return Promise.resolve();
  }
}

globalThis.playOneShot = function(filename) {
  try {
    const audio = getAudio(filename);
    audio.currentTime = 0;
    audio.loop = false;
    playAudioSafely(audio, err => console.warn('Failed to play audio:', err));
  } catch (err) {
    console.error('Error playing one-shot sound:', err);
  }
};

globalThis.playLoop = function(filename) {
  try {
    const audio = getAudio(filename);
    
    if (userHasInteracted) {
      const existingLoop = loopingAudio.get(filename);
      if (existingLoop && !existingLoop.paused) {
        existingLoop.pause();
      }
      
      audio.currentTime = 0;
      audio.loop = true;
      
      loopingAudio.set(filename, audio);
      
      audio.play().catch(err => {
        console.warn('Failed to play looped audio:', err);
        loopingAudio.delete(filename);
      });
    } else {
      pendingLoopedAudio.set(filename, () => {
        const existingLoop = loopingAudio.get(filename);
        if (existingLoop && !existingLoop.paused) {
          existingLoop.pause();
        }
        
        const freshAudio = getAudio(filename);
        freshAudio.currentTime = 0;
        freshAudio.loop = true;
        loopingAudio.set(filename, freshAudio);
        
        return freshAudio.play().catch(err => {
          console.warn('Failed to play queued looped audio:', err);
          loopingAudio.delete(filename);
        });
      });
    }
  } catch (err) {
    console.error('Error playing looped sound:', err);
  }
};

globalThis.pauseSound = function(filename) {
  try {
    const loopingAudioElement = loopingAudio.get(filename);
    if (loopingAudioElement) {
      loopingAudioElement.pause();
      return;
    }
    
    const audio = audioCache.get(filename);
    if (audio) {
      audio.pause();
    }
  } catch (err) {
    console.error('Error pausing sound:', err);
  }
};

globalThis.resumeSound = function(filename) {
  try {
    const loopingAudioElement = loopingAudio.get(filename);
    if (loopingAudioElement) {
      playAudioSafely(loopingAudioElement, err => console.warn('Failed to resume looped audio:', err));
      return;
    }
    
    const audio = audioCache.get(filename);
    if (audio) {
      playAudioSafely(audio, err => console.warn('Failed to resume audio:', err));
    }
  } catch (err) {
    console.error('Error resuming sound:', err);
  }
};

if (typeof window !== 'undefined') {
  window.playOneShot = globalThis.playOneShot;
  window.playLoop = globalThis.playLoop;
  window.pauseSound = globalThis.pauseSound;
  window.resumeSound = globalThis.resumeSound;
}
