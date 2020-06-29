/*******************************************************************************
*** WASM iface
*******************************************************************************/

const Wordament = Object.defineProperties({}, {
  STATES: {
    get: function() {
      return {
        WORDLIST: 1,
        BOARD: 1 << 1
      }
    }
  },
  version: {
    get: function() {
      return Module.cwrap('version', 'int', []);
    }
  },
  getState: {
    get: function() {
      return Module.cwrap('get_state', 'int', []);
    }
  },
  state: {
    get: function() {
      const s = {};
      const _s = this.getState();
      for (let key in this.STATES) {
        s[key] = (_s & this.STATES[key]) !== 0;
      }
      return s;
    }
  },
  loadWordlist: {
    get: function() {
      return Module.cwrap('load_wordlist', 'number', ['string']);
    }
  },
  printWordlist: {
    get: function() {
      return Module.cwrap('print_wordlist', 'number', ['number']);
    }
  },
  loadBoard: {
    get: function() {
      return Module.cwrap('load_board', 'number', ['string']);
    }
  },
  search: {
    get: function() {
      return function() {
        const wordlist = Module.cwrap('search', 'number', [])();
        return new Wordlist(wordlist);
      }
    }
  },
  setDebug: {
    get: function() {
      return Module.cwrap('set_debug', null, ['number']);
    }
  }
})

class Wordlist {
  constructor(pointer) {
    this.pointer = pointer;
  }
  [Symbol.iterator]() {
    const readWord = Module.cwrap('wordlist_readword', 'string', ['number']);
    const readPoints = Module.cwrap('wordlist_readpoints', 'number', ['number']);
    const nextWord = Module.cwrap('wordlist_nextword', 'number', ['number']);
    let node = this.pointer;
    let i = 0;
    return {
      next: function() {
        node = nextWord(node);
        if (node === 0) return { value: i, done: true };
        i++;
        return { value: {
          word: readWord(node),
          points: readPoints(node)
        }, done: false };
      }
    }
  }
  toArray() {
    return Array.from(this);
  }
  log() {
    for (let word of this) {
      console.log(word);
    }
  }
}

/*******************************************************************************
*** Events
*******************************************************************************/

window.addEventListener('click', function(e) {
  const classes = e.target.classList.length;
  for (let i=0;i<classes;i++) {
    switch(e.target.classList.item(i)) {
      case 'input-submit':
      console.log('Solve');
      break;
    }
  }
});

window.addEventListener('load', function() {
  fetch('./assets/wordlist.txt', { mode: 'same-origin', credentials: 'omit' }).then(function(res) {
    console.log(res);
    res.text().then(function(text) {
      console.log('Wordlist downloaded...');
      Wordament.setDebug(1 | 1 << 1);
      const s = Wordament.loadWordlist(text);
      if (s !== 0) console.error('Error parsing wordlist');
    }, console.error);
  }, console.error);
}, { once: true });
