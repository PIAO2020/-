//logs.js
const util = require('../../utils/util.js')

Page({
  data: {
    logs: []
  },
  xj: function () {
    wx.navigateTo({
      url: '../xj/xj',
    })
  },
  onLoad: function () {
    this.setData({
      logs: (wx.getStorageSync('logs') || []).map(log => {
        return util.formatTime(new Date(log))
      })
    })
  }
})
