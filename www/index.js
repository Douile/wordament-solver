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
      return Module.cwrap('search', 'number', []);
    }
  }
})
