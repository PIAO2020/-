// miniprogram/pages/inmax/inmax.js
Page({
  data: {
    ne:[],
  },
  xianshi: function (options) {
    let that=this
    wx.cloud.database().collection('cun').get({
      success(res){
        that.setData({
          ne:res.data
        }),
        console.log(res.data)
      }
    })
   },       
  shuaxin: function (options) {
    　　const db = wx.cloud.database();
    　　db.collection('cun').get().then(res => {
    　　　　console.log(res); 
    　　　　var that = this;
    　　　　that.setData({
    　　　　　　listDatas: res.data
    　　　　});
    　　　　console.log(res.data);
    　　}).catch(err => {
    　　　　console.log(err); 
    　　})
    },

  onLoad: function (options) {

  },  
  onReady: function () {

  },
  onShow: function () {

  },
  onHide: function () {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {

  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {

  }
})