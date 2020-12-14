// pages/xj/xj.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
   time:"",
   thing:"",
  },
  time:function(e){
        this.setData({
          time:e.detail.value
        })
      },
     chaos:function(e){
          this.setData({
            thing:e.detail.value
          })
        },
       post:function(n){
           var that=this;
           wx.request({
            method:'POST',
             url: 'http://api.heclouds.com/devices/652113157/datapoints',
             header:{
               'api-key':'7cn4RQ2nAnRluXyDXSarRtD=buA='
             },
             data: {
              "datastreams": [{
                      "id": "jishiben",
                      "datapoints": [{
                            "value": that.data.time,
                          }
                      ]
                  }]
                },
             success:function(e){
             console.log(e)
            },
          })
         },
         post1:function(n){
           var that=this;
          wx.request({
           method:'POST',
            url: 'http://api.heclouds.com/devices/652113157/datapoints',
            header:{
              'api-key':'7cn4RQ2nAnRluXyDXSarRtD=buA='
            },
            data: {
             "datastreams": [{
                     "id": "jishiben1",
                     "datapoints": [{
                           "value": that.data.thing,
                         }
                     ]
                 }]
               },
            success:function(e){
            console.log(e)
           },
         })
        },
  /**
   * 生命周期函数--监听页面加载
   */
  res: function(e) {
    const db = wx.cloud.database()
    db.collection('cun').add({
      data: {
        time: e.detail.value.time ,
        thing: e.detail.value.thing
      },
      success: res => {
        // 在返回结果中会包含新创建的记录的 _id
        this.setData({
          time: e.detail.value.time,
          thing: e.detail.value.thing
        })
        wx.showToast({
          title: '新增记录成功',
        })
        console.log('[数据库] [新增记录] 成功，记录 _id: ', res._id)
      },
      fail: err => {
        wx.showToast({
          icon: 'none',
          title: '新增记录失败'
        })
        console.error('[数据库] [新增记录] 失败：', err)
      }
    })
  },
  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {

  },

  /**
   * 生命周期函数--监听页面隐藏
   */
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
  