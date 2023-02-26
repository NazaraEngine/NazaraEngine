package com.sirlynix.nazaraExample

import android.app.NativeActivity
import android.os.Bundle
import android.widget.TextView
import com.sirlynix.nazaraExample.androidTest.databinding.ActivityMainBinding

class MainActivity : NativeActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method
        binding.sampleText.text = stringFromJNI()
    }

    /**
     * A native method that is implemented by the 'NazaraApp' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'NazaraApp' library on application startup.
        init {
            System.loadLibrary("NazaraApp")
        }
    }
}