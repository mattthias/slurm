require "formula"


class Slurm < Formula
  homepage "https://github.com/mattthias/slurm"
  url "https://github.com/returned/slurm/archive/upstream.zip"
  sha1 "aea3235e382e192f6b239a6b65d7768079999a38"


  version "0.4.0"

  depends_on :x11
  depends_on "scons" => :build

  def install

    system "scons"
    bin.install "slurm"

  end

  test do

    system "slurm"
  end
end
